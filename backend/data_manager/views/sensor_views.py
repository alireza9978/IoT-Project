from rest_framework import mixins, status
from rest_framework.permissions import AllowAny
from rest_framework.response import Response
from rest_framework.viewsets import GenericViewSet

from data_manager.models import SensorData
from data_manager.serializers import SensorSerializer


class SensorReceiveDataViewSet(mixins.CreateModelMixin, GenericViewSet):
    permission_classes = [AllowAny]
    serializer_class = SensorSerializer
    queryset = SensorData.objects.none()

    def create(self, request, *args, **kwargs):
        try:
            for data in request.data['data']:
                try:
                    serializer = self.get_serializer(data=data)
                    serializer.is_valid(raise_exception=True)
                    self.perform_create(serializer)
                except Exception as e:
                    print(e.args)
        except Exception as e:
            return Response(data={"errors": e.args}, status=status.HTTP_400_BAD_REQUEST)
        return Response(status=status.HTTP_200_OK)
