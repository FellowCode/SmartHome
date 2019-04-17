from datetime import datetime

from django.http import HttpResponse
from django.shortcuts import render, redirect

from Account.models import ExtendedUser, generate_key
from .forms import *
from Device.models import Termocontroller
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.models import User
from SMTP.tasks import *

def signup(request):
    if request.method == 'POST':
        form = SignUpForm(request.POST)
        if form.is_valid():
            user = User.objects.create(username=form.cleaned_data['email'].lower(),
                                       email=form.cleaned_data['email'].lower(),
                                       password=form.cleaned_data['password'])
            user.first_name = form.cleaned_data['first_name']
            user.last_name = form.cleaned_data['last_name']
            user.save()
            login(request, user)
            send_confirm_mail(user)
            return redirect('/')
        return render(request, 'Account/SignUp.html', {'form': form})
    return render(request, 'Account/SignUp.html')

def signin(request):
    if request.method == 'POST':
        if not request.user.is_authenticated:
            form = SignInForm(request.POST)
            if form.is_valid():
                user = authenticate(username=form.cleaned_data['email'].lower(), password=form.cleaned_data['password'])
                if user:
                    login(request, user)
                    return redirect('/')
                return render(request, 'Account/SignIn.html', {'form': form, 'error': 'Неверный логин или пароль'})
            return render(request, 'Account/SignIn.html', {'form': form, 'error': 'Неверный логин или пароль'})
        return redirect('/')
    data = {'email_confirm': False, 'password_restored': False}
    if 'email_confirm' in request.session:
        data['email_confirm'] = True
        del request.session['email_confirm']
    if 'password_restored' in request.session:
        data['password_restored'] = True
        del request.session['password_restored']
    return render(request, 'Account/SignIn.html', data)

def user_logout(request):
    if request.user.is_authenticated:
        logout(request)
    return redirect('/')

def user_account(request):
    import SmartHome.utils as utils
    import SmartHome.views as main
    user = request.user
    if user and user.is_authenticated:
        device_list = Termocontroller.objects.filter(user=user)
        for device in device_list:
            # if device.id in utils.clients_cm_buffer:
            #     utils.clients_cm_buffer[device.id] += 'UpdateData\n'
            if main.server and device.id in main.server.clients_by_id:
                main.server.clients_by_id[device.id].send_update_request()
        ext_user = user.extendeduser
        email_confirm = False
        if 'email_confirm' in request.session:
            email_confirm = True
            del request.session['email_confirm']
        return render(request, 'Account/UserAccount.html', {'device_list': device_list, 'ext_user': ext_user, 'email_confirm': email_confirm})
    return redirect('/account/signin/')

def email_confirm(request, user_id, token):
    user = User.objects.filter(id=user_id).first()
    if user and token == user.extendeduser.token:
        timedif = datetime.now() - user.extendeduser.token_date.replace(tzinfo=None)
        if timedif.days < 1:
            user.extendeduser.is_confirmed = True
            user.save()
            request.session['email_confirm'] = True
            if request.user.is_authenticated:
                return redirect('/account/')
            else:
                return redirect('/account/signin/')
    return HttpResponse(status=404)

def email_confirm_again(request):
    user = request.user
    if user.is_authenticated:
        user.extendeduser.token_date = datetime.now()
        user.extendeduser.save()
        send_confirm_mail(user)
        return render(request, 'Account/MailSent.html')
    return redirect('/account/signin/')

def restore_password(request):
    if request.method == 'POST':
        form = RestorePassForm(request.POST)
        if form.is_valid():
            user = User.objects.get(username=form.cleaned_data['email'])
            user.extendeduser.restore_token = generate_key(length=16)
            user.extendeduser.restore_token_date = datetime.now()
            user.extendeduser.save()
            send_restore_pass_mail(user)
            return render(request, 'Account/MailSent.html', {'email': user.username})
        return render(request, 'Account/RestorePassword.html', {'form': form})
    return render(request, 'Account/RestorePassword.html')

def restore_pass_confirm(request, user_id, token):
    user = User.objects.filter(id=user_id).first()
    if user:
        if token == user.extendeduser.restore_token:
            timedif = datetime.now() - user.extendeduser.restore_token_date.replace(tzinfo=None)
            if hours(timedif) < 1:
                if request.method == 'POST':
                    form = RestorePassConfirmForm(request.POST)
                    if form.is_valid():
                        user.set_password(form.cleaned_data['password'])
                        user.save()
                        if user.is_authenticated:
                            logout(request)
                        request.session['password_restored'] = True
                        user.extendeduser.restore_token = generate_key(length=16)
                        return redirect('/account/signin/')
                    return render(request, 'Account/RestorePassConfirm.html', {'form': form})
                return render(request, 'Account/RestorePassConfirm.html')
    return HttpResponse(status=404)


def hours(td):
    return td.seconds//3600
