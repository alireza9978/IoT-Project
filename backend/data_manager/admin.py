from django.contrib import admin

from data_manager.models import SensorData


@admin.register(SensorData)
class SensorDataAdmin(admin.ModelAdmin):
    model = SensorData
    list_display = ['sensor', 'temperature', 'humidity', 'brightness', 'time']
