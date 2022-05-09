from rest_framework.routers import DefaultRouter

from data_manager.views.sensor_views import SensorReceiveDataViewSet

router = DefaultRouter()

router.register('new_sensor_data', SensorReceiveDataViewSet, basename='new_sensor_data')


urlpatterns = [
] + router.urls
