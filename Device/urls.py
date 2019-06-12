from django.contrib import admin
from django.urls import path, include
from .views import *

urlpatterns = [
    path('change/', change_termo),
    path('settings/<id>/', settings),
    path('stat/<id>/', statistic),
]