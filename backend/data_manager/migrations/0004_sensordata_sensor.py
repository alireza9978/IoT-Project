# Generated by Django 3.2.13 on 2022-05-09 08:35

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('gateway', '0004_auto_20220509_1305'),
        ('data_manager', '0003_remove_sensordata_sensor'),
    ]

    operations = [
        migrations.AddField(
            model_name='sensordata',
            name='sensor',
            field=models.ForeignKey(default=1, on_delete=django.db.models.deletion.CASCADE, to='gateway.sensor', verbose_name='sensor'),
            preserve_default=False,
        ),
    ]
