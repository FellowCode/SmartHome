from django.shortcuts import render, redirect
from .forms import *
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login

def signup(request):
    if request.method == 'POST':
        form = SignUpForm(request.POST)
        if form.is_valid():
            user = User.objects.create_user(username=form.cleaned_data['email'].lower(),
                                       email=form.cleaned_data['email'],
                                       password=form.cleaned_data['password'])
            user.first_name = form.cleaned_data['first_name']
            user.last_name = form.cleaned_data['last_name']
            user.save()
            login(request, user)
            return redirect('/')
        else:
            return render(request, 'Account/SignUp.html', {'form': form})
    else:
        return render(request, 'Account/SignUp.html')

def signin(request):
    if request.method == 'POST':
        form = SignInForm(request.POST)
        if form.is_valid():
            user = authenticate(username=form.cleaned_data['email'].lower(), password=form.cleaned_data['password'])
            if user:
                login(request, user)
                return redirect('/')
            else:
                return render(request, 'Account/SignIn.html', {'form': form, 'error': 'Неверный логин или пароль'})
        else:
            return render(request, 'Account/SignIn.html', {'form': form, 'error': 'Неверный логин или пароль'})
    else:
        return render(request, 'Account/SignIn.html')
