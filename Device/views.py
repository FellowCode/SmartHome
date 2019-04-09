from datetime import datetime
from django.contrib.auth import authenticate
from django.http import HttpResponse
from django.shortcuts import render, redirect
from django.views.decorators.csrf import csrf_protect, csrf_exempt
from .models import *

@csrf_exempt
def set_data(request):
    if request.method == 'POST':
        print(str(request.POST))
        termo_c = None
        data = {}
        answer = 'Auth error'
        user = authenticate(username=request.POST['AccountLogin'], password=request.POST['AccountPassword'])
        if user:
            answer = 'Success'
            if (request.POST.get('init', None)):
                termo_c = create_termo(request.POST, user)
                data['string_id'] = termo_c.string_id
            if not termo_c:
                try:
                    termo_c = Termocontroller.objects.get(string_id=str(request.POST['string_id']))
                    if (request.POST.get('ChangeTargetTemp', None) == 'True'):
                        termo_c.target_temp = float(request.POST['target_temp'])
                except:
                    termo_c = create_termo(request.POST, user)
                    data['string_id'] = termo_c.string_id
            termo_c.temp = float(request.POST['temp'])
            termo_c.humidity = float(request.POST['humidity'])
            termo_c.KWatts = float(request.POST['KWatts'])
            termo_c.save()
            if float(request.POST['target_temp']) != float(termo_c.target_temp):
                data['target_temp'] = termo_c.target_temp
            if len(data) > 0:
                answer = 'Change configuration:'
                l = []
                for key, val in data.items():
                    l.append(str(key) + '=' + str(val))
                answer += ';'.join(l)
            print(answer)
        return render(request, 'device_data.html', {'data': answer})
    print('not post')
    return HttpResponse(status=404)


def create_termo(req, user):
    model_name = req['model_name']
    try:
        name = TermocontrollerName.objects.get(name=model_name)
    except:
        name = TermocontrollerName.objects.create(name=model_name)
        name.save()
    termo_c = Termocontroller.objects.create()
    termo_c.user = user
    termo_c.model_name = name
    termo_c.target_temp = float(req['target_temp'])
    return termo_c
