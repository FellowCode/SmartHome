import socket
import sys
from threading import Thread
from time import sleep
import select
import queue
import logging
from datetime import datetime

from django.contrib.auth import authenticate

from Device.models import TermocontrollerName, Termocontroller
from SmartHome.settings import *
import json


# Логирование
logging.basicConfig(filename='server.log', level=logging.DEBUG)

class Server(Thread):
    clients = {}

    inputs = []
    outputs = []

    def __init__(self):
        super().__init__()
        self.start_server()

    def start_server(self):
        host = SERVER_ADDRESS  # 62.109.29.169
        port = SERVER_PORT  # arbitrary non-privileged port

        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.setblocking(False)
        self.server.settimeout(0)

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
            readable, writable, exceptional = select.select(
                self.inputs, self.outputs, self.inputs)
            for s in readable:
                # Входящие данные

                if s is self.server:
                    # Если от сокета сервера, принимаем подключение
                    connection, client_address = s.accept()
                    connection.setblocking(False)
                    connection.settimeout(0)
                    print('Conn adress: ', ':'.join(map(str, client_address)))
                    self.inputs.append(connection)
                    self.outputs.append(connection)
                    self.clients[connection] = Client(self, connection, client_address)
                else:
                    # Если от сокета клиента, принимаем данные
                    try:
                        s.setblocking(False)
                        s.settimeout(0)
                        data = s.recv(2048)
                        if data:
                            if data != b'disconnect':
                                self.clients[s].get_data_handler(data)
                            else:
                                self.client_disconnect(s)
                    except Exception:
                        print(sys.exc_info())
                        # При ошибке отключаем клиента
                        self.client_disconnect(s)
            for s in writable:
                # Исходящие данные
                try:
                    next_msg = self.clients[s].q.get_nowait()
                except queue.Empty:
                    pass
                else:
                    s.send(next_msg)

            for s in exceptional:
                # При ошибке на сокете исключаем его
                self.client_disconnect(s)
            sleep(0.01)

    def get_client_list(self):
        addresses = []
        for conn, client in self.clients.items():
            addresses.append(client.get_adress())
        return addresses

    def find_client_by_id(self, id):
        for conn, client in self.clients.items():
            if client.id == id:
                return client


    #Отключение клиента
    def client_disconnect(self, s):
        if s in self.outputs:
            self.outputs.remove(s)
        self.inputs.remove(s)
        s.close()
        print(self.clients[s].get_address() + ' disconnected')
        del self.clients[s]

    def stop_server(self):
        self.server.close()
        self.inputs = None


class Client:
    work = True
    connection = None
    id = None

    CM_NEED_DATA = 'SendCurrentData'
    CM_CHANGE_CONFIG = 'ChangeConfig:'

    def __init__(self, server, connection, address):
        self.server = server
        self.connection = connection
        self.ip = address[0]
        self.port = address[1]
        self.q = queue.Queue()

    def get_data_handler(self, data):
        print(self.get_address() + ' > ' + str(data))
        data = data.decode('utf-8')
        data = data.strip().split(':')
        command = data[0]
        if command == 'ChangeValues':
            request = {}
            field_list = data[1].split('&')
            for field in field_list:
                key, value = field.split('=')
                request[key] = value
            self.change_termo_values(request)


    def change_termo_values(self, request):
        termo_c = None
        data = {}
        answer = 'AuthError'
        user = authenticate(username=request['AccountLogin'], password=request['AccountPassword'])
        if user:
            if ('init' in request and request['init'] == 'True'):
                termo_c = self.create_termo(request, user)
                data['string_id'] = termo_c.string_id
            if not termo_c:
                try:
                    termo_c = Termocontroller.objects.get(string_id=str(request['string_id']))
                    if (request['ChangeTargetTemp'] == 'True'):
                        termo_c.target_temp = float(request['target_temp'])
                except:
                    termo_c = self.create_termo(request, user)
                    data['string_id'] = termo_c.string_id
            termo_c.temp = float(request['temp'])
            termo_c.humidity = float(request['humidity'])
            termo_c.KWatts = float(request['KWatts'])
            termo_c.save()
            if float(request['target_temp']) != float(termo_c.target_temp):
                data['target_temp'] = termo_c.target_temp
            if len(data) > 0:
                self.change_termo_config(data)
            self.id = termo_c.id
            return
        self.send(answer.encode())

    def create_termo(self, request, user):
        model_name = request['model_name']
        try:
            name = TermocontrollerName.objects.get(name=model_name)
        except:
            name = TermocontrollerName.objects.create(name=model_name)
            name.save()
        termo_c = Termocontroller.objects.create()
        termo_c.user = user
        termo_c.model_name = name
        termo_c.target_temp = float(request['target_temp'])
        return termo_c

    def change_termo_config(self, data):
        command = self.CM_CHANGE_CONFIG
        l = []
        for key, val in data.items():
            l.append(str(key) + '=' + str(val))
        command += ';'.join(l)
        self.send(command.encode())

    def send_update_request(self):
        command = self.CM_NEED_DATA
        self.send(command.encode())

    #data - байты
    def send(self, data):
        print(data)
        # if not self.connection in self.server.outputs:
        #     self.server.outputs.append(self.connection)
        self.q.put(data + b'\n')

    def get_address(self):
        return str(self.ip) + ':' + str(self.port)

