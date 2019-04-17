from django.http import HttpResponse
from django.shortcuts import render_to_response, render
from django.template import RequestContext
from SocketTCP.server import Server, Client
from SmartHome.settings import *
from Device.models import Termocontroller

server = None

def handler404(request, *args, **argv):
    response = render_to_response('404.html')
    response.status_code = 404
    return response


def handler500(request, *args, **argv):
    response = render_to_response('500.html')
    response.status_code = 500
    return response

def start_server(request):
    global server
    if not server:
        for termo_c in Termocontroller.objects.all():
            termo_c.is_connected = False
            termo_c.save()
        server = Server()
        server.start()
        return render(request, 'device_data.html', {'data': 'Server started!'})
    return HttpResponse(status=404)
