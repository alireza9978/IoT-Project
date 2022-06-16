from rest_framework import serializers
from rest_framework.permissions import IsAuthenticated
from rest_framework.viewsets import ModelViewSet

from gateway.models import Sensor


class SensorModelViewSet(ModelViewSet):
    class SensorSerializer(serializers.ModelSerializer):
        class Meta:
            model = Sensor
            exclude = ['user']

    permission_classes = [IsAuthenticated]
    queryset = Sensor.objects.none()
    serializer_class = SensorSerializer

    def get_queryset(self):
        return Sensor.objects.filter(user=self.request.user)

    def perform_create(self, serializer):
        serializer.save(user=self.request.user)
