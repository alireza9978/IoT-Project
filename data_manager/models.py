from django.db import models
from django.utils.translation import gettext_lazy as _

from gateway.models import Sensor


class SensorData(models.Model):
    sensor = models.ForeignKey(Sensor, on_delete=models.CASCADE, verbose_name=_("sensor"))
    temperature = models.IntegerField(verbose_name=_("temperature"))
    humidity = models.IntegerField(verbose_name=_("humidity"))
    brightness = models.IntegerField(verbose_name=_("brightness"))
    created_at = models.DateTimeField(auto_now_add=True, verbose_name=_("created at"))

    class Meta:
        verbose_name = _("sensor data")
        verbose_name_plural = _("sensor data")

    def __str__(self):
        return f'{self.sensor} - {self.created_at}'
