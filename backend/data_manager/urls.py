from rest_framework.routers import DefaultRouter

from data_manager.views.sensor_views import SensorReceiveDataViewSet, SensorDataView

router = DefaultRouter()

router.register('new_sensor_data', SensorReceiveDataViewSet, basename='new_sensor_data')
router.register('sensor_data', SensorDataView, basename='sensor_data')

urlpatterns = [
] + router.urls
