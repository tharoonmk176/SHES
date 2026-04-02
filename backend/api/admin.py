from django.contrib import admin

from .models import Log


@admin.register(Log)
class LogAdmin(admin.ModelAdmin):
    list_display = ("timestamp", "event", "relay", "ldr", "motion")
    list_filter = ("relay", "motion")
    search_fields = ("event",)

# Register your models here.
