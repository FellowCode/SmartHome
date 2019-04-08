from django.contrib.auth.models import User
from django.db import models
import uuid


class Termocontroller(models.Model):

    string_id = models.CharField(max_length=50, null=True, blank=True, unique=True)

    user = models.ForeignKey(User, on_delete=models.CASCADE, blank=True, null=True)

    model_name = models.ForeignKey('TermocontrollerName', on_delete=models.PROTECT, blank=True, null=True)

    temp = models.DecimalField(max_digits=4, decimal_places=1, null=True, blank=True)
    target_temp = models.DecimalField(max_digits=4, decimal_places=1, null=True, blank=True)

    humidity = models.DecimalField(max_digits=4, decimal_places=1, null=True, blank=True)

    KWatts = models.DecimalField(max_digits=10, decimal_places=1, null=True, blank=True)

    time_get_data = models.DateTimeField(auto_now=True)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        if not self.string_id:
            self.string_id = str(uuid.uuid4())
            print('generate')


class TermocontrollerName(models.Model):
    name = models.CharField(max_length=50, unique=True)

    def __str__(self):
        return self.name
