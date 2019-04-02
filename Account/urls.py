from django.contrib import admin
from django.urls import path, include
from .views import *

urlpatterns = [
    path('signup/', signup),
    path('signin/', signin),
    path('logout/', user_logout),
    path('', user_account)
]