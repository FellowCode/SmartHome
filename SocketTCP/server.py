import socket
import sys
from threading import Thread
from time import sleep
import select
import queue
import logging
from datetime import datetime
import hashlib

from Account.models import ExtendedUser
from django.contrib.auth.models import User

from django.contrib.auth import authenticate

from Device.models import TermocontrollerName, Termocontroller, TermocontrollerFirmware, TermocontStatistic
from SmartHome.settings import *
import json
import SmartHome.utils as utils




class Server(Thread):
    clients = {}
    clients_by_id = {}

    inputs = []
    outputs = []

    def __init__(self):
        super().__init__()
        self.start_server()
        self.daemon = True

    def start_server(self):
        host = SERVER_ADDRESS  # 62.109.29.169
        port = SERVER_PORT  # arbitrary non-privileged port

        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.setblocking(False)

        try:
            self.server.bind((host, port))
        except:
            logging.info('Error: %r', str(sys.exc_info()))
            print("Bind failed. Error : " + str(sys.exc_info()))
            sys.exit()

        self.server.listen(1000)  # queue up to 1000 requests
        print("Socket now listening")
        self.inputs.append(self.server)
        logging.info('Server started: %r', datetime.now().strftime('%Y.%m.%d %H:%M'))

    def run(self):
        # Мониторинг сокетов

        while self.inputs:
            #Проверка не была ли добавлена команда для клиента
            # for device_id, command in utils.clients_cm_buffer.items():
            #     if len(command) > 0:
            #         utils.clients_cm_buffer[device_id] = self.clients_by_id[device_id].new_command(command)

            readable, writable, exceptional = select.select(
                self.inputs, self.outputs, self.inputs, 0.005)
            for s in readable:
                # Входящие данные

                if s is self.server:
                    # Если от сокета сервера, принимаем подключение
                    connection, client_address = s.accept()
                    connection.setblocking(False)
                    print('Conn adress: ', ':'.join(map(str, client_address)))
                    self.inputs.append(connection)
                    client = Client(self, connection, client_address)
                    self.clients[connection] = client
                else:
                    # Если от сокета клиента, принимаем данные
                    try:
                        data = s.recv(2048)
                    except:
                        print(sys.exc_info())
                        # При ошибке отключаем клиента
                        self.client_disconnect(s)
                    else:
                        if data:
                            if data != b'disconnect':
                                self.clients[s].get_data_handler(data)
                            else:
                                self.client_disconnect(s)

            for s in writable:
                # Исходящие данные
                try:
                    if not self.clients[s].q.empty():
                        next_msg = self.clients[s].q.get_nowait()
                        s.send(next_msg)
                    else:
                        self.outputs.remove(s)
                except:
                    self.client_disconnect(s)

            for s in exceptional:
                # При ошибке на сокете исключаем его
                self.client_disconnect(s)
            sleep(0.005)

    def get_client_list(self):
        addresses = []
        for conn, client in self.clients.items():
            addresses.append(client.get_adress())
        return addresses


    #Отключение клиента
    def client_disconnect(self, s):
        if s in self.outputs:
            self.outputs.remove(s)
        self.inputs.remove(s)
        s.close()
        print(self.clients[s].get_address() + ' disconnected')
        self.clients[s].on_disconnect()
        del self.clients[s]


    def stop_server(self):
        self.server.close()
        self.inputs = None


class Client:
    work = True
    connection = None
    id = None


    buffer = ''

    CM_NEED_DATA = 'SendCurrentData'
    CM_CHANGE_CONFIG = 'ChangeConfig:'

    def __init__(self, server, connection, address):
        self.server = server
        self.connection = connection
        self.ip = address[0]
        self.port = address[1]
        self.q = queue.Queue()

    def get_data_handler(self, data):
        self.buffer += data.decode('utf-8')
        while '\r\n' in self.buffer:
            data = self.buffer[:self.buffer.find('\r\n')]
            self.buffer = self.buffer[self.buffer.find('\r\n')+2:]
            print(data)
            data_s = data.strip().split(':')
            command = data_s[0]
            if command == 'ChangeValues':
                check_str = data[:data.rfind('&')]
                request = {}
                field_list = data_s[1].split('&')
                for field in field_list:
                    key, value = field.split('=')
                    request[key] = value
                self.change_termo_values(request, check_str)


    def change_termo_values(self, request, check_str):
        data = {}
        answer = 'AuthError'
        user = User.objects.filter(extendeduser__api_key=request['api_key']).first()
        if user:
            #Проверка данных хешированием с секретным ключом
            hash_str = check_str + ';' + user.extendeduser.secret_key
            hash = hashlib.sha1(hash_str.encode()).hexdigest()
            if request['hash'] == hash:
                termo_c = Termocontroller.objects.filter(string_id=str(request['string_id'])).first()
                if termo_c:
                    if request['ChangeTargetTemp'] == 'True':
                        termo_c.target_temp = float(request['target_temp'])
                    if request['ChangeEnable'] == 'True':
                        termo_c.enabled = request['enable'] == 'True'
                else:
                    termo_c = self.create_termo(request)
                    data['string_id'] = termo_c.string_id
                firmwware, created = TermocontrollerFirmware.objects.get_or_create(name=request['firmware_version'])
                if created:
                    firmwware.save()
                termo_c.firmware_version = firmwware
                termo_c.temp = float(request['temp'])
                termo_c.humidity = float(request['humidity'])
                termo_c.KWatts = float(request['KWatts'])
                if termo_c.enabled != (request['enable'] == 'True'):
                    data['enable'] = termo_c.enabled
                if float(request['target_temp']) != float(termo_c.target_temp):
                    data['target_temp'] = termo_c.target_temp
                if len(data) > 0:
                    self.change_termo_config(user, **data)
                if not self.id:
                    self.id = termo_c.id
                    self.server.clients_by_id[self.id] = self
                    utils.clients_cm_buffer[self.id] = ''
                    termo_c.is_connected = True
                    termo_c.user = user
                termo_c.save()
                stat = TermocontStatistic.objects.create()
                stat.device = termo_c
                stat.temp = termo_c.temp
                stat.humidity = termo_c.humidity
                stat.save()
                return
            answer = 'HashCheckError'
        self.send(answer)

    def create_termo(self, request):
        model_name = request['model_name']
        firmware_version = request['firmware_version']
        termo_name, created = TermocontrollerName.objects.get_or_create(name=model_name)
        if created:
            termo_name.save()
        termo_firmware, created = TermocontrollerFirmware.objects.get_or_create(name=firmware_version)
        if created:
            termo_firmware.save()
        termo_c = Termocontroller.objects.create()
        termo_c.model_name = termo_name
        termo_c.firmware_version = termo_firmware
        termo_c.target_temp = float(request['target_temp'])
        return termo_c

    def change_termo_config(self, user, **data):
        command = self.CM_CHANGE_CONFIG
        l = []
        for key, val in data.items():
            l.append(str(key) + '=' + str(val))
        command += '&'.join(l)
        #добавление хэша
        data = '{0};{1}'.format(command, user.extendeduser.secret_key)
        data = '{0}&hash={1}'.format(command, hashlib.sha1(data.encode()).hexdigest())
        self.send(data)

    def on_disconnect(self):
        if self.id:
            del self.server.clients_by_id[self.id]
            termo_c = Termocontroller.objects.filter(id=self.id).first()
            if termo_c:
                termo_c.is_connected = False
                termo_c.save()

    def send_update_request(self):
        command = self.CM_NEED_DATA
        self.send(command)

    #data - байты
    def send(self, data):
        if type(data) == str:
            data = data.encode()
        print('send:', data)
        # if not self.connection in self.server.outputs:
        #     self.server.outputs.append(self.connection)
        self.q.put(data + b'\n')
        if not self.connection in self.server.outputs:
            self.server.outputs.append(self.connection)

    def new_command(self, command):
        buffer = command[command.find('\n')+1:]
        command = command[:command.find('\n')]

        if command == 'UpdateData':
            self.send_update_request()

        return buffer

    def get_address(self):
        return str(self.ip) + ':' + str(self.port)

