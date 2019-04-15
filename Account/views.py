from django.shortcuts import render, redirect

from Account.models import ExtendedUser
from .forms import *
from Device.models import Termocontroller
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.models import User
from SMTP.tasks import send_confirm_mail

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
    data = {'email_confirm': False}
    if 'email_confirm' in request.session:
        data['email_confirm'] = True
        del request.session['email_confirm']
    return render(request, 'Account/SignIn.html', data)

def user_logout(request):
    if request.user.is_authenticated:
        logout(request)
    return redirect('/')


def user_account(request):
    import Main.views as main

    user = request.user
    if user and user.is_authenticated:
        device_list = Termocontroller.objects.filter(user=user)
        for device in device_list:
            client = main.server.find_client_by_id(device.id)
            if client:
                client.send_update_request()
        ext_user = user.extendeduser
        email_confirm = False
        if 'email_confirm' in request.session:
            email_confirm = True
            del request.session['email_confirm']
        return render(request, 'Account/UserAccount.html', {'device_list': device_list, 'ext_user': ext_user, 'email_confirm': email_confirm})
    return redirect('/')

def email_confirm(request, user_id, token):
    user = User.objects.filter(id=user_id).first()
    if user and token == user.extendeduser.token:
        user.extendeduser.is_confirmed = True
        user.save()
        request.session['email_confirm'] = True
        if request.user.is_authenticated:

            return redirect('/account/')
        else:
            return redirect('/account/signin/')
    return redirect('/')

