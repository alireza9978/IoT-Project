from rest_framework import serializers

from data_manager.models import SensorData


class SensorSerializer(serializers.ModelSerializer):
    class Meta:
        model = SensorData
        fields = ['sensor', 'temperature', 'humidity', 'brightness', 'time']

