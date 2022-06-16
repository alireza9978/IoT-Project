from django.db import models
from django.utils.translation import gettext_lazy as _

from users.models import User


class Sensor(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE, verbose_name=_("user"))
    mac = models.CharField(primary_key=True, max_length=150, unique=True, verbose_name=_("mac address"))

    class Meta:
        verbose_name = _("sensor")
        verbose_name_plural = _("sensors")

    def __str__(self):
        return self.mac
