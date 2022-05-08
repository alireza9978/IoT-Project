from django.contrib import admin

from gateway.models import Gateway, Sensor


@admin.register(Gateway)
class GatewayAdmin(admin.ModelAdmin):
    model = Gateway
    list_display = ['id', 'mac_address']


@admin.register(Sensor)
class SensorAdmin(admin.ModelAdmin):
    model = Sensor
    list_display = ['id', 'gateway']
