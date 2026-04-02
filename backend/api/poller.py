import os
import threading
import time

import requests

from .models import Log


ESP32_BASE_URL = os.getenv("ESP32_BASE_URL", "http://localhost:8180")


def poll_once(last_event: str | None) -> str | None:
    try:
        response = requests.get(f"{ESP32_BASE_URL}/data", timeout=3)
        response.raise_for_status()
        data = response.json()
    except requests.RequestException:
        return last_event

    event = data.get("lastEvent")
    if event and event != last_event:
        Log.objects.create(
            event=event,
            relay=bool(data.get("relay")),
            ldr=int(data.get("ldr", 0)),
            motion=bool(data.get("motion")),
        )
        return event

    return last_event


def run_poller(interval_seconds: int = 3) -> None:
    last_event: str | None = None
    while True:
        last_event = poll_once(last_event)
        time.sleep(interval_seconds)


def start_poller_thread(interval_seconds: int = 3) -> None:
    thread = threading.Thread(
        target=run_poller,
        kwargs={"interval_seconds": interval_seconds},
        daemon=True,
        name="esp32-poller",
    )
    thread.start()
