from django.shortcuts import render
from SocketTCP.server import Server

server = Server()
server.start()

def index(request):
    return render(request, 'Main/Index.html')
