from django.shortcuts import render
from SocketTCP.server import Server
from SmartHome.settings import *

if SERVER_ON:
    server = Server()
    server.start()

def index(request):
    return render(request, 'Main/Index.html')
