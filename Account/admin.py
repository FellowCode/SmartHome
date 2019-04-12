from django.contrib import admin
from django.contrib.auth.models import User, Group
from .models import *

admin.site.unregister(Group)

@admin.register(ExtendedUser)
class ExtendedUserAdmin(admin.ModelAdmin):
    pass
