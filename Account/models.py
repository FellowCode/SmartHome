from django.db import models
from datetime import datetime
from django.utils import timezone
from django.contrib.auth.models import User
import random
import string
from django.dispatch import receiver
from django.db.models.signals import post_save
from SmartHome.settings import *

class ExtendedUser(models.Model):
    user = models.OneToOneField(User, on_delete=models.CASCADE)

    api_key = models.CharField(max_length=32, unique=True)

    secret_key = models.CharField(max_length=32)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        #Генерирование уникального api ключа
        if not self.api_key:
            while True:
                api_key = ''
                for i in range(10):
                    api_key += random.choice(SYMBOLS_FOR_KEY)
                # Проверка на уникальность
                if not ExtendedUser.objects.filter(api_key=api_key).exists():
                    self.api_key = api_key
                    break

        if not self.secret_key:
            secret_key = ''
            for i in range(10):
                secret_key += random.choice(SYMBOLS_FOR_KEY)
            self.secret_key = secret_key

    def __str__(self):
        return str(self.user.username)


@receiver(post_save, sender=User)
def create_user_profile(sender, instance, created, **kwargs):
    if created:
        ExtendedUser.objects.create(user=instance)

@receiver(post_save, sender=User)
def save_user_profile(sender, instance, **kwargs):
    instance.extendeduser.save()
