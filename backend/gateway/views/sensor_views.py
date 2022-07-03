from django.db.models import Sum
from rest_framework import serializers
from rest_framework.permissions import IsAuthenticated
from rest_framework.viewsets import ModelViewSet

from gateway.models import Sensor


class SensorModelViewSet(ModelViewSet):
    class SensorSerializer(serializers.ModelSerializer):
        sum = serializers.SerializerMethodField()

        class Meta:
            model = Sensor
            exclude = ['user']

        def get_sum(self, instance):
            return instance.sensordata_set.all().aggregate(sum=Sum('energy'))['sum']

    permission_classes = [IsAuthenticated]
    queryset = Sensor.objects.all()
    serializer_class = SensorSerializer
    filterset_fields = ['user']

    # def get_queryset(self):
    #     return Sensor.objects.filter(user=self.request.user)

    def perform_create(self, serializer):
        serializer.save(user=self.request.user)
