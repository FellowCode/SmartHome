from datetime import datetime
from django.contrib.auth import authenticate
from django.http import HttpResponse
from django.shortcuts import render, redirect
from django.views.decorators.csrf import csrf_protect, csrf_exempt
from .models import *


def change_termo(requset):
    import SmartHome.utils as utils
    if requset.method == 'POST':
        print(requset.POST)
        id = requset.POST['id']
        termo_c = Termocontroller.objects.get(id=id)
        termo_c.enabled = requset.POST['enable'] == 'true'
        termo_c.target_temp = float(requset.POST['target_temp'])
        termo_c.save()
        if termo_c.id in utils.clients_cm_buffer and termo_c.is_connected:
            utils.clients_cm_buffer[termo_c.id] += 'UpdateData\n'
        return HttpResponse(status=200)
    return HttpResponse(status=404)
