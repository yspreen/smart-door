from clock import clock

OPEN_DELTA = 2.0  # seconds
CLOSED_DELTA = 2.0  # seconds


class HingeManager:
    def __init__(self):
        self.pin_open = False
        self.pin_open_since = clock.get_time()
        self.pin_closed_since = clock.get_time()
        self.was_open_for_some_time = False

    def is_open_for_some_time(self):
        return self.pin_open and (self.pin_open_since + OPEN_DELTA < clock.get_time())

    def is_closed_for_some_time(self):
        return (not self.pin_open) and (
            self.pin_closed_since + CLOSED_DELTA < clock.get_time()
        )

    def tick(self, door_is, door_should_be, hinge_open):
        was_open = self.pin_open
        self.pin_open = hinge_open

        if was_open != hinge_open:
            if hinge_open:
                self.pin_open_since = clock.get_time()
            else:
                self.pin_closed_since = clock.get_time()

        if self.is_open_for_some_time():
            self.was_open_for_some_time = True
        if self.is_closed_for_some_time() and self.was_open_for_some_time:
            door_is = True
            door_should_be = False
            self.was_open_for_some_time = False

        return door_is, door_should_be, self.is_open_for_some_time()
