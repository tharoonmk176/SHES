from django.db import models


class Log(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    event = models.CharField(max_length=255)
    relay = models.BooleanField()
    ldr = models.IntegerField()
    motion = models.BooleanField()

    def __str__(self) -> str:
        return f"{self.timestamp.isoformat()} - {self.event}"

# Create your models here.
