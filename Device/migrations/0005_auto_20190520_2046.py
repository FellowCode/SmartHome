# Generated by Django 2.2 on 2019-05-20 10:46

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('Device', '0004_termocontroller_is_connected'),
    ]

    operations = [
        migrations.CreateModel(
            name='TermocontrollerFirmware',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=50, unique=True)),
            ],
        ),
        migrations.RemoveField(
            model_name='termocontroller',
            name='firmware_version',
        ),
    ]
