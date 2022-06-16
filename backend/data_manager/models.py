from django.db import models
from django.utils.translation import gettext_lazy as _

from gateway.models import Sensor


class SensorData(models.Model):
    sensor = models.ForeignKey(Sensor, on_delete=models.CASCADE, verbose_name=_("sensor"))
    voltage = models.FloatField(verbose_name=_("voltage"))
    current = models.FloatField(verbose_name=_("voltage"))

    time = models.CharField(max_length=150, verbose_name=_("time"))

    class Meta:
        verbose_name = _("sensor data")
        verbose_name_plural = _("sensor data")

    def __str__(self):
        return f'{self.sensor} - {self.time}'
