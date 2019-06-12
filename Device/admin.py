from django.contrib import admin
from .models import *

@admin.register(Termocontroller)
class TermocontrollerAdmin(admin.ModelAdmin):
    readonly_fields = ['time_get_data']

@admin.register(TermocontrollerName)
class TermocontrollerNameAdmin(admin.ModelAdmin):
    pass

@admin.register(TermocontStatistic)
class TermocontStatAdmin(admin.ModelAdmin):
    readonly_fields = ['device', 'date', 'temp', 'humidity']
