from django.shortcuts import render, redirect
from .forms import *
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout

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
    return render(request, 'Account/SignIn.html')

def user_logout(request):
    if request.user.is_authenticated:
        logout(request)
    return redirect('/')


def user_account(request):
    user = request.user
    if user.is_authenticated:
        return render(request, 'Account/UserAccount.html')
    return redirect('/')
