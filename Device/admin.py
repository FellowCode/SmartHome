from django.contrib import admin
from .models import *

@admin.register(Termocontroller)
class TermocontrollerAdmin(admin.ModelAdmin):
    pass

@admin.register(TermocontrollerName)
class TermocontrollerAdmin(admin.ModelAdmin):
    pass
