from django.urls import path
from rest_framework.routers import DefaultRouter

from data_manager.views.sensor_views import SensorReceiveDataViewSet, SensorDataView, CumulativeDiagramView

router = DefaultRouter()

router.register('new_sensor_data', SensorReceiveDataViewSet, basename='new_sensor_data')
router.register('sensor_data', SensorDataView, basename='sensor_data')

urlpatterns = [
    path('cumulative_digram/', CumulativeDiagramView.as_view())
] + router.urls
