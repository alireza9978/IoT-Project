from django.db import models
from django.utils.translation import gettext_lazy as _


class Gateway(models.Model):
    name = models.CharField(max_length=150, null=True, blank=True, verbose_name=_("name"))
    mac_address = models.CharField(unique=True, max_length=150, verbose_name=_("mac address"))

    class Meta:
        verbose_name = _("gateway")
        verbose_name_plural = _("gateways")

    def __str__(self):
        return self.mac_address


class Sensor(models.Model):
    gateway = models.ForeignKey(Gateway, on_delete=models.CASCADE, verbose_name=_("gateway"), related_name="sensors")

    class Meta:
        verbose_name = _("sensor")
        verbose_name_plural = _("sensors")

    def __str__(self):
        return f'{self.gateway} - {self.id}'
