from rest_framework.permissions import BasePermission

from gateway.models import Gateway


class IsGateway(BasePermission):
    def has_permission(self, request, view):
        # return bool(
        #     hasattr(request.data, '')
        # )
        mac_address = request.META.get('HTTP_MAC', None)
        if not mac_address:
            return False
        try:
            gateway = Gateway.objects.get(mac_address=mac_address)
        except Gateway.DoesNotExist:
            return False
        return True
