import sys
if __name__ != "__main__":
    print("This must be run as the main file.")

import pygame
from virtual_comms import VirtualComms
from pong import Pong

pygame.init()

#Setup the window
screen_size = (1200, 24)
screen = pygame.display.set_mode(screen_size)
pygame.display.set_caption("1D Pong")

clock = pygame.time.Clock()

prev_keys = pygame.key.get_pressed()
curr_keys = pygame.key.get_pressed()

NUM_LEDS = 120
LED_SIZE = (screen_size[0] // NUM_LEDS, screen_size[1])

vcomms = VirtualComms(NUM_LEDS)
vpong = Pong(vcomms, NUM_LEDS)
vpong.R_MAX = 255
vpong.G_MAX = 255
vpong.B_MAX = 255

vpong.setup()
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit(0)

    vpong.update()
    vpong.draw()

    screen.fill((0, 0, 0))
    for j, p in enumerate(vcomms.leds):
        pygame.draw.rect(screen, p.toTuple(), pygame.Rect((LED_SIZE[0] * j, 0), LED_SIZE))

    pygame.display.update()
    clock.tick(60)