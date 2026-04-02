from django.core.management.base import BaseCommand

from api.poller import run_poller


class Command(BaseCommand):
    help = "Run the ESP32 polling loop that stores log changes."

    def add_arguments(self, parser) -> None:
        parser.add_argument(
            "--interval",
            type=int,
            default=3,
            help="Polling interval in seconds (default: 3).",
        )

    def handle(self, *args, **options) -> None:
        interval = options["interval"]
        self.stdout.write(self.style.SUCCESS(f"Starting ESP32 poller (every {interval}s)..."))
        run_poller(interval_seconds=interval)
