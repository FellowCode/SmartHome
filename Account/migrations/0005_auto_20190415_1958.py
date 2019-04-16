# Generated by Django 2.2 on 2019-04-15 09:58

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('Account', '0004_auto_20190415_1958'),
    ]

    operations = [
        migrations.AlterField(
            model_name='extendeduser',
            name='api_key',
            field=models.CharField(default='M1sYRRtZhP', max_length=32, unique=True),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='restore_token',
            field=models.CharField(default='rSpC2HggNwTPKARD', max_length=32),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='restore_token_date',
            field=models.DateTimeField(auto_now_add=True),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='secret_key',
            field=models.CharField(default='RAJMg3JjkZ', max_length=32),
        ),
        migrations.AlterField(
            model_name='extendeduser',
            name='token',
            field=models.CharField(default='nfQBWNtpo7vw930J', max_length=32),
        ),
    ]
