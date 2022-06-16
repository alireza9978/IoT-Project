from django.contrib import admin

from gateway.models import Sensor


@admin.register(Sensor)
class SensorAdmin(admin.ModelAdmin):
    model = Sensor
    list_display = ['mac']
