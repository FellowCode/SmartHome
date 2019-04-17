from datetime import datetime
from django.contrib.auth import authenticate
from django.http import HttpResponse
from django.shortcuts import render, redirect
from django.views.decorators.csrf import csrf_protect, csrf_exempt
from .models import *
from .forms import *


def change_termo(requset):
    import SmartHome.utils as utils
    import SmartHome.views as main
    if requset.method == 'POST':
        print(requset.POST)
        data = {}
        id = requset.POST['id']
        termo_c = Termocontroller.objects.get(id=id)
        termo_c.enabled = requset.POST['enable'] == 'true'
        if termo_c.enabled:
            data['enable'] = 'True'
        else:
            data['enable'] = 'False'
        termo_c.target_temp = float(requset.POST['target_temp'])
        data['target_temp'] = requset.POST['target_temp']
        termo_c.save()
        # if termo_c.id in utils.clients_cm_buffer and termo_c.is_connected:
        #     utils.clients_cm_buffer[termo_c.id] += 'UpdateData\n'
        if main.server and termo_c.id in main.server.clients_by_id:
            main.server.clients_by_id[termo_c.id].change_termo_config(termo_c.user, **data)
        return HttpResponse(status=200)
    return HttpResponse(status=404)

def settings(request, id):
    user = request.user

    if user.is_authenticated:
        termo_c = Termocontroller.objects.get(id=id)
        if termo_c.user == user:
            if request.method == 'POST':
                form = SettingsForm(request.POST)
                if form.is_valid():
                    termo_c.custom_name = form.cleaned_data['custom_name']
                    termo_c.save()
                    request.session['settings_changed'] = True
                    return redirect('/account/')
                return render(request, 'Device/Settings.html', {'form': form})
            form = SettingsForm(initial={'custom_name': termo_c.custom_name})
            return render(request, 'Device/Settings.html', {'form': form})
        return redirect('/account/')
    return redirect('/account/signin/')
