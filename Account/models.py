from django.db import models
from datetime import datetime
from django.utils import timezone
from django.contrib.auth.models import User
import random
import string
from django.dispatch import receiver
from django.db.models.signals import post_save
from SmartHome.settings import *

def generate_key(length, unique_field='', model=None):
    key = ''
    for i in range(length):
        key += random.choice(SYMBOLS_FOR_KEY)
    filter = {unique_field: key}
    if len(unique_field) > 0 and model:
        while model.objects.filter(filter).exists():
            key = generate_key(length)
            filter = {unique_field: key}
    return key

class ExtendedUser(models.Model):

    user = models.OneToOneField(User, on_delete=models.CASCADE)

    is_confirmed = models.BooleanField(default=False)

    #подтверждение почты
    token = models.CharField(max_length=32, default=generate_key(length=16))
    token_date = models.DateTimeField(auto_now_add=True)

    #восстановление пароля
    restore_token = models.CharField(max_length=32, default=generate_key(length=16))
    restore_token_date = models.DateTimeField(auto_now_add=True)

    api_key = models.CharField(max_length=32, unique=True, default=generate_key(length=10, unique_field='api_key'))

    secret_key = models.CharField(max_length=32, default=generate_key(length=10))

    def __str__(self):
        return str(self.user.username)

def generate_key_tmp(**kwargs):
    kwargs['model'] = ExtendedUser
    return generate_key(**kwargs)


@receiver(post_save, sender=User)
def create_user_profile(sender, instance, created, **kwargs):
    if created:
        ExtendedUser.objects.create(user=instance)

@receiver(post_save, sender=User)
def save_user_profile(sender, instance, **kwargs):
    instance.extendeduser.save()
