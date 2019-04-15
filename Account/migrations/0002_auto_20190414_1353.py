# Generated by Django 2.2 on 2019-04-14 03:53

import datetime
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('Account', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='extendeduser',
            name='is_confirmed',
            field=models.BooleanField(default=False),
        ),
        migrations.AddField(
            model_name='extendeduser',
            name='token',
            field=models.CharField(default='QkzxfGbSXwNr777N', max_length=32),
        ),
        migrations.AddField(
            model_name='extendeduser',
            name='token_date',
            field=models.DateTimeField(default=datetime.datetime(2019, 4, 14, 13, 53, 35, 929947)),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='api_key',
            field=models.CharField(default='f57whrM6SJ', max_length=32, unique=True),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='secret_key',
            field=models.CharField(default='g2fw3iYRrV', max_length=32),
        ),
    ]
