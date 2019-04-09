import socket
import sys
from threading import Thread
from time import sleep
import select
import queue
import logging
from datetime import datetime

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
        host = "localhost"  # 62.109.29.169
        port = 9999  # arbitrary non-privileged port

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
            readable, writable, exceptional = select.select(
                self.inputs, self.outputs, self.inputs)
            for s in readable:
                # Входящие данные
                if s is self.server:
                    # Если от сокета сервера, принимаем подключение
                    connection, client_address = s.accept()
                    connection.setblocking(0)
                    print('Conn adress: ', ':'.join(map(str, client_address)))
                    self.inputs.append(connection)
                    self.clients[connection] = Client(self, connection, client_address)
                else:
                    # Если от сокета клиента, принимаем данные
                    try:
                        data = s.recv(1024)
                        if data:
                            if data != b'disconnect':
                                self.clients[s].get_data_handler(data)
                            else:
                                self.client_disconnect(s)
                    except:
                        # При ошибке отключаем клиента
                        self.client_disconnect(s)

            for s in writable:
                # Исходящие данные
                try:
                    next_msg = self.clients[s].q.get_nowait()
                except queue.Empty:
                    self.outputs.remove(s)
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

    def __init__(self, server, connection, address):
        self.server = server
        self.connection = connection
        self.ip = address[0]
        self.port = address[1]
        self.q = queue.Queue()

    def get_data_handler(self, data):
        print(self.get_address() + ' > ' + str(data))
        self.send(b'Ok')

    def send(self, data):
        if not self.connection in self.server.outputs:
            self.server.outputs.append(self.connection)
        self.q.put(data)

    def get_address(self):
        return str(self.ip) + ':' + str(self.port)

