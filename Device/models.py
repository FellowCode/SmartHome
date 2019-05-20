from django.contrib.auth.models import User
from django.db import models
from Account.models import ExtendedUser
import uuid
import datetime


class Termocontroller(models.Model):

    string_id = models.CharField(max_length=50, null=True, blank=True, unique=True)

    is_connected = models.BooleanField(default=False)

    enabled = models.BooleanField(default=True)

    user = models.ForeignKey(User, on_delete=models.CASCADE, blank=True, null=True)

    model_name = models.ForeignKey('TermocontrollerName', on_delete=models.PROTECT, blank=True, null=True)

    firmware_version = models.ForeignKey('TermocontrollerFirmware', on_delete=models.PROTECT, blank=True, null=True)

    custom_name = models.CharField(max_length=100, blank=True, null=True)

    temp = models.DecimalField(max_digits=4, decimal_places=1, null=True, blank=True)
    target_temp = models.DecimalField(max_digits=4, decimal_places=1, null=True, blank=True)

    humidity = models.DecimalField(max_digits=4, decimal_places=1, null=True, blank=True)

    KWatts = models.DecimalField(max_digits=10, decimal_places=1, null=True, blank=True)

    time_get_data = models.DateTimeField(auto_now=True)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        if not self.string_id:
            self.string_id = str(uuid.uuid4())

    def save(self, *args, **kwargs):
        if (not self.custom_name) or self.custom_name == '' or self.custom_name == 'None':
            self.custom_name = str(self.model_name)
        super(Termocontroller, self).save(*args, **kwargs)

    def __str__(self):
        return str(self.user.username) + ' - ' + str(self.custom_name)



class TermocontrollerName(models.Model):
    name = models.CharField(max_length=50, unique=True)

    def __str__(self):
        return self.name


class TermocontrollerFirmware(models.Model):
    name = models.CharField(max_length=50, unique=True)

    def __str__(self):
        return self.name

class TermocontStatistic(models.Model):
    device = models.ForeignKey(Termocontroller, on_delete=models.CASCADE)

    date = models.DateTimeField(auto_now_add=True)

    temp = models.DecimalField(max_digits=4, decimal_places=1)
    humidity = models.DecimalField(max_digits=4, decimal_places=1)

    def __str__(self):
        return str(self.device.id) + str(datetime.datetime.strptime(self.date, '%m/%d/%y %H:%M:%S'))
