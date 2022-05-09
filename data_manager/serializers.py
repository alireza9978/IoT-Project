from rest_framework import serializers

from data_manager.models import SensorData


class SensorSerializer(serializers.ModelSerializer):
    class Meta:
        model = SensorData
        fields = '__all__'
        read_only_fields = ['created_at']

