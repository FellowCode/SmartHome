from datetime import datetime
from django.contrib.auth import authenticate
from django.http import HttpResponse
from django.shortcuts import render, redirect
from django.views.decorators.csrf import csrf_protect, csrf_exempt
from .models import *

def change_termo(requset):
    if requset.method == 'POST':
        import Main.views as main
        print(requset.POST)
        id = requset.POST['id']
        termo_c = Termocontroller.objects.get(id=id)
        termo_c.enabled = requset.POST['enable'] == 'true'
        termo_c.target_temp = float(requset.POST['target_temp'])
        termo_c.save()
        client = main.server.find_client_by_id(termo_c.id)
        if client:
            client.send_update_request()
        return HttpResponse(status=200)
    return HttpResponse(status=404)
