from django.shortcuts import render
from SocketTCP.server import Server


def index(request):
    return render(request, 'Main/Index.html')
