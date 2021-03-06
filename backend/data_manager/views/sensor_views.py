from datetime import datetime, timedelta

from django.db.models import Sum
from django.utils.translation import gettext_lazy as _
from rest_framework import mixins, status, serializers
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response
from rest_framework.views import APIView
from rest_framework.viewsets import GenericViewSet, ModelViewSet

from data_manager.models import SensorData
from gateway.models import Sensor


class SensorReceiveDataViewSet(mixins.CreateModelMixin, GenericViewSet):
    time = serializers.CharField(required=True)
    energy = serializers.FloatField(required=True)

    class SensorDataInputSerializer(serializers.Serializer):
        class Meta:
            # model = SensorData
            fields = ['energy', 'time']

    # def validate_time(self, value):
    #     try:
    #         print("1111111")
    #         return datetime.fromtimestamp(value)
    #     except Exception as e:
    #         print("2222")
    #         raise serializers.ValidationError(e.args)

    permission_classes = [IsAuthenticated]
    serializer_class = SensorDataInputSerializer
    queryset = SensorData.objects.none()

    def create(self, request, *args, **kwargs):
        try:
            sensor, temp = Sensor.objects.get_or_create(mac=request.data['sensor_mac'], user=request.user)
            if sensor.user != self.request.user:
                return Response(data={"errors": _("you do not have access to this sensor")},
                                status=status.HTTP_400_BAD_REQUEST)
            for data in request.data['sensor_data']:
                try:
                    serializer = self.get_serializer(data=data)
                    serializer.is_valid(raise_exception=True)
                    SensorData.objects.create(sensor=sensor, energy=float(data['energy']),
                                              time=datetime.fromtimestamp(int(data['time'])))
                    # serializer.save(sensor=sensor)
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

    queryset = SensorData.objects.all().order_by('-time')
    permission_classes = [IsAuthenticated]
    serializer_class = SensorDataSerializer
    filterset_fields = ['sensor', 'sensor__user']


class CumulativeDiagramView(APIView):
    def get(self, request, *args, **kwargs):
        try:
            sensor = Sensor.objects.get(mac=request.GET['mac'])
            start_time = datetime.now() - timedelta(days=2)
            now = datetime.now()
            result = []
            while now > start_time:
                data = {}
                end_time = start_time + timedelta(hours=1)
                data.setdefault('time', end_time)
                sum = SensorData.objects.filter(sensor=sensor, time__gte=start_time, time__lt=end_time).aggregate(
                    sum=Sum('energy'))['sum'] or 0
                data.setdefault('sum', sum)
                result.append(data)
                start_time = end_time
            return Response(result, status=status.HTTP_200_OK)
        except Exception as e:
            return Response(data={"error": e.args}, status=status.HTTP_400_BAD_REQUEST)
