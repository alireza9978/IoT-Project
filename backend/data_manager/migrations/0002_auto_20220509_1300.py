# Generated by Django 3.2.13 on 2022-05-09 08:30

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('data_manager', '0001_initial'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='sensordata',
            name='created_at',
        ),
        migrations.AddField(
            model_name='sensordata',
            name='time',
            field=models.CharField(default=1, max_length=150, verbose_name='time'),
            preserve_default=False,
        ),
    ]
