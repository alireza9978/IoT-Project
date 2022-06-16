# Generated by Django 3.2.13 on 2022-06-16 10:58

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('data_manager', '0004_sensordata_sensor'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='sensordata',
            name='brightness',
        ),
        migrations.RemoveField(
            model_name='sensordata',
            name='humidity',
        ),
        migrations.RemoveField(
            model_name='sensordata',
            name='temperature',
        ),
        migrations.AddField(
            model_name='sensordata',
            name='current',
            field=models.FloatField(default=1, verbose_name='voltage'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='sensordata',
            name='voltage',
            field=models.FloatField(default=1, verbose_name='voltage'),
            preserve_default=False,
        ),
    ]