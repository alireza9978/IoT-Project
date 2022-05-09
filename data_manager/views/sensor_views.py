from rest_framework import mixins
from rest_framework.viewsets import GenericViewSet

from data_manager.models import SensorData
from data_manager.serializers import SensorSerializer
from gateway.permissions import IsGateway


class SensorReceiveDataViewSet(mixins.CreateModelMixin, GenericViewSet):
    permission_classes = [IsGateway]
    serializer_class = SensorSerializer
    queryset = SensorData.objects.none()

