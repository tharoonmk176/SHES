import os

import requests
from rest_framework import status
from rest_framework.decorators import api_view
from rest_framework.response import Response

from .models import Log
from .serializers import LogSerializer


ESP32_BASE_URL = os.getenv("ESP32_BASE_URL", "http://localhost:8180")


@api_view(["GET"])
def data_view(_request):
    try:
        response = requests.get(f"{ESP32_BASE_URL}/data", timeout=3)
        response.raise_for_status()
        return Response(response.json())
    except requests.RequestException as exc:
        return Response({"error": str(exc)}, status=status.HTTP_502_BAD_GATEWAY)


@api_view(["GET", "POST"])
def logs_view(request):
    if request.method == "GET":
        logs = Log.objects.order_by("-timestamp")[:200]
        return Response(LogSerializer(logs, many=True).data)

    serializer = LogSerializer(data=request.data)
    if serializer.is_valid():
        log = serializer.save()
        return Response(LogSerializer(log).data, status=status.HTTP_201_CREATED)
    return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


@api_view(["POST"])
def relay_view(request):
    state = request.query_params.get("state")
    if state not in {"on", "off"}:
        return Response(
            {"error": "state must be 'on' or 'off'"},
            status=status.HTTP_400_BAD_REQUEST,
        )

    try:
        response = requests.post(
            f"{ESP32_BASE_URL}/relay",
            params={"state": state},
            timeout=3,
        )
        response.raise_for_status()
    except requests.RequestException as exc:
        return Response({"error": str(exc)}, status=status.HTTP_502_BAD_GATEWAY)

    try:
        payload = response.json()
    except ValueError:
        payload = {"message": response.text}

    return Response(payload, status=response.status_code)

# Create your views here.
