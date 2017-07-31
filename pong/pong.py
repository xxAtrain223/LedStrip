import sys
import collections
import copy
import time
from rgba import RGBA

sys.path.append("..")
from Messenger.LedStripComms import LedStripMessenger

class Pong(object):
    def __init__(self, comms):
        self.comms = comms

        self.NUM_LEDS = 120
        INITIAL_SCORE = 5
        self.MAX_SCORE = 10

        self.R_MAX = 127
        self.G_MAX = 63
        self.B_MAX = 31

        self.onstrip = [None for i in range(self.NUM_LEDS)]
        self.background = [RGBA(0, 0, 0) for i in range(self.NUM_LEDS)]
        self.foreground = [RGBA(0, 0, 0, 0) for i in range(self.NUM_LEDS)]

        self.score_a = INITIAL_SCORE
        self.score_b = INITIAL_SCORE

        self.ball_pos = self.NUM_LEDS // 2
        self.ball_dir = 1

    def fillSolid(self, pixel):
        self.comms.fillSolid(pixel.rp(), pixel.gp(), pixel.bp())
        self.onstrip = [pixel] * self.NUM_LEDS

    def setPixel(self, i, pixel):
        if self.onstrip[i] != pixel:
            self.onstrip[i] = copy.deepcopy(pixel)
            self.comms.setPixel(i, pixel.rp(), pixel.gp(), pixel.bp())

    def setup(self):
        self.comms.ping()
        self.comms.pauseCalculations()

        self.fillSolid(RGBA(0))

        self.draw_zone_a()
        self.draw_zone_b()


    def draw_zone_a(self):
        for i in range(0, self.score_a):
            r = int((self.score_a - 1 - i) / self.score_a * self.R_MAX)
            g = int(                    i  / self.score_a * self.G_MAX)
            self.background[i] = RGBA(r, g, 0)

        self.background[self.MAX_SCORE] = RGBA(255, 0, 255)

    def draw_zone_b(self):
        for i in range(0, self.score_b):
            r = int((self.score_b - 1 - i) / self.score_b * self.R_MAX)
            g = int(                    i  / self.score_b * self.G_MAX)
            self.background[self.NUM_LEDS - 1 - i] = RGBA(r, g, 0)

        self.background[self.NUM_LEDS - 1 - self.MAX_SCORE] = RGBA(0, 255, 255)

    def update(self):
        self.foreground[self.ball_pos] = RGBA(0, a=0)
        self.ball_pos = self.ball_pos + self.ball_dir
        if self.ball_pos < 0:
            self.ball_pos = 0
            self.ball_dir = 1
        elif self.ball_pos >= self.NUM_LEDS:
            self.ball_pos = self.NUM_LEDS - 1
            self.ball_dir = -1
        self.foreground[self.ball_pos] = RGBA(255, a=255)

    def draw(self):
        for i in range(self.NUM_LEDS):
            self.setPixel(i, self.background[i] + self.foreground[i])

if __name__ == "__main__":
    with LedStripMessenger("/dev/LedStripController") as comms:
        p = Pong(comms)
        p.setup()
        while True:
            p.update()
            p.draw()
