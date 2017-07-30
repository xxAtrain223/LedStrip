import sys
import collections
import copy
import time

sys.path.append("..")
from Messenger.LedStripComms import LedStripMessenger

def clamp(n, smallest, largest):
    return max(smallest, min(n, largest))

class rgba(object):
    def __init__(self, r, g=None, b=None, a=255):
        self.r = int(clamp(r, 0, 255))
        if g != None and b != None:
            self.g = int(clamp(g, 0, 255))
            self.b = int(clamp(b, 0, 255))
        else:
            self.g = self.r
            self.b = self.r
        self.a = int(clamp(a, 0, 255))

    def rf(self):
        return self.r / 255.0

    def gf(self):
        return self.g / 255.0

    def bf(self):
        return self.b / 255.0

    def af(self):
        return self.a / 255.0

    def rp(self):
        return int(self.rf() * self.af() * 255)

    def gp(self):
        return int(self.gf() * self.af() * 255)

    def bp(self):
        return int(self.bf() * self.af() * 255)

    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return self.__dict__ == other.__dict__
        return False

    def __ne__(self, other):
        return not self.__eq__(other)

    def __repr__(self):
        return "rgba(r={}, g={}, b={}, a={})".format(self.r, self.g, self.b, self.a)

    def __add__(self, other):
        if not isinstance(other, self.__class__):
            raise TypeError("Unsupported operand type(s) for +: '{}' and '{}'".format(self.__class__, type(other)))

        a = other.af() + self.af() * (1 - other.af())
        r = (other.rf() * other.af() + self.rf() * self.af() * (1 - other.af())) / a
        g = (other.gf() * other.af() + self.gf() * self.af() * (1 - other.af())) / a
        b = (other.bf() * other.af() + self.bf() * self.af() * (1 - other.af())) / a

        return rgba(r * 255, g * 255, b * 255, a * 255)

with LedStripMessenger("/dev/LedStripController") as comms:
    comms.ping()
    comms.pauseCalculations()

    NUM_LEDS = 120
    r_max = 127
    g_max = 63
    b_max = 31

    def setPixel(i, pixel):
        #print(i, onstrip[i], pixel, onstrip[i] != pixel)
        if onstrip[i] != pixel:
            onstrip[i] = copy.deepcopy(pixel)
            comms.setPixel(i, pixel.rp(), pixel.gp(), pixel.bp())

    def fillSolid(pixel):
        comms.fillSolid(pixel.rp(), pixel.gp(), pixel.bp())
        onstrip = [pixel] * NUM_LEDS

    background = [rgba(0, 0, 0) for i in range(NUM_LEDS)]
    foreground = [rgba(0, 0, 0, 0) for i in range(NUM_LEDS)]
    onstrip = copy.deepcopy(background)

    fillSolid(rgba(0))

    size = 10
    for i in range(0, size):
        background[i].r = int((size - 1 - i) / size * r_max)
        background[i].g = int(            i  / size * g_max)
        
        background[-i-1].r = int((size - 1 - i) / size * r_max)
        background[-i-1].g = int(            i  / size * g_max)

    background[20].r = 255
    background[20].b = 255
    background[-21].g = 255
    background[-21].b = 255

    i = NUM_LEDS // 2
    dir = 1
    while True:
        foreground[i] = rgba(0, a=0)
        i = i + dir
        if i < 0:
            i = 0
            dir = 1
        elif i >= NUM_LEDS:
            i = NUM_LEDS - 1
            dir = -1
        foreground[i] = rgba(255, a=255)

        for index in range(NUM_LEDS):
            setPixel(index, background[index] + foreground[index])
