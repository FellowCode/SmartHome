# Generated by Django 2.2 on 2019-04-12 10:20

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('Device', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='termocontroller',
            name='enabled',
            field=models.BooleanField(default=True),
        ),
    ]
