# Generated by Django 2.2 on 2019-04-16 09:36

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('Account', '0006_auto_20190415_2024'),
    ]

    operations = [
        migrations.AlterField(
            model_name='extendeduser',
            name='api_key',
            field=models.CharField(default='eQBWOJOKaY', max_length=32, unique=True),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='restore_token',
            field=models.CharField(default='jM2KKPHy8ap8HNVH', max_length=32),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='secret_key',
            field=models.CharField(default='7YA4hSd4Hy', max_length=32),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='token',
            field=models.CharField(default='1r3s6nPubHheZniq', max_length=32),
        ),
    ]
