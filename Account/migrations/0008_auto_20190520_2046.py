# Generated by Django 2.2 on 2019-05-20 10:46

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('Account', '0007_auto_20190416_1936'),
    ]

    operations = [
        migrations.AlterField(
            model_name='extendeduser',
            name='api_key',
            field=models.CharField(default='SjyL2w6z7z', max_length=32, unique=True),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='restore_token',
            field=models.CharField(default='TUsim3JDzkqxXTHy', max_length=32),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='secret_key',
            field=models.CharField(default='O6d746CjsX', max_length=32),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='token',
            field=models.CharField(default='dseHS0q2EssSFsix', max_length=32),
        ),
    ]
