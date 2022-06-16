from rest_framework.routers import DefaultRouter

from gateway.views.sensor_views import SensorModelViewSet

router = DefaultRouter()

router.register('sensors', SensorModelViewSet, basename='sensor')


urlpatterns = [

] + router.urls
