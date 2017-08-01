from rgba import RGBA

class VirtualComms(object):
    def __init__(self, num_leds):
        self.NUM_LEDS = num_leds
        self.leds = [RGBA(0, 0, 0) for i in range(self.NUM_LEDS)]

    def ping(self):
        pass

    def pauseCalculations(self):
        pass

    def fillSolid(self, r, g, b):
        self.leds = [RGBA(r, g, b) for i in range(self.NUM_LEDS)]

    def setPixel(self, i, r, g ,b):
        self.leds[i] = RGBA(r, g, b)
