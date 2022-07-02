from django.utils.translation import gettext_lazy as _
from rest_framework import mixins, status, serializers
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response
from rest_framework.viewsets import GenericViewSet, ModelViewSet

from data_manager.models import SensorData
from gateway.models import Sensor


class SensorReceiveDataViewSet(mixins.CreateModelMixin, GenericViewSet):
    class SensorDataInputSerializer(serializers.ModelSerializer):
        class Meta:
            model = SensorData
            fields = ['energy', 'time']

    permission_classes = [IsAuthenticated]
    serializer_class = SensorDataInputSerializer
    queryset = SensorData.objects.none()

    def create(self, request, *args, **kwargs):
        try:
            sensor = Sensor.objects.get(mac=request.data['sensor_mac'])
            if sensor.user != self.request.user:
                return Response(data={"errors": _("you do not have access to this sensor")},
                                status=status.HTTP_400_BAD_REQUEST)
            for data in request.data['sensor_data']:
                try:
                    serializer = self.get_serializer(data=data)
                    serializer.is_valid(raise_exception=True)
                    serializer.save(sensor=sensor)
                except Exception as e:
                    print(e.args)
        except Exception as e:
            return Response(data={"errors": e.args}, status=status.HTTP_400_BAD_REQUEST)
        return Response(status=status.HTTP_200_OK)


class SensorDataView(ModelViewSet):
    class SensorDataSerializer(serializers.ModelSerializer):
        class Meta:
            model = SensorData
            fields = '__all__'
    queryset = SensorData.objects.all()
    permission_classes = [IsAuthenticated]
    serializer_class = SensorDataSerializer
    filterset_fields = ['sensor', 'sensor__user']
