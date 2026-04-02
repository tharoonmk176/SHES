from django.urls import path

from . import views

urlpatterns = [
    path("data/", views.data_view, name="data"),
    path("logs/", views.logs_view, name="logs"),
    path("relay/", views.relay_view, name="relay"),
]
