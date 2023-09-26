import utime


class Clock:
    def __init__(self):
        self._last_ticks = utime.ticks_us()
        self._total_ticks = 0

    def update(self):
        current_ticks = utime.ticks_us()
        delta_ticks = utime.ticks_diff(current_ticks, self._last_ticks)
        self._last_ticks = current_ticks
        self._total_ticks += delta_ticks

    def get_time(self):
        self.update()
        return self._total_ticks / 1_000_000  # Convert to seconds


clock = Clock()
