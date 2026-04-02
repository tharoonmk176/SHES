import os

from django.apps import AppConfig


class ApiConfig(AppConfig):
    default_auto_field = "django.db.models.BigAutoField"
    name = "api"

    def ready(self) -> None:
        if os.getenv("START_POLLER") == "1":
            from .poller import start_poller_thread

            start_poller_thread()
