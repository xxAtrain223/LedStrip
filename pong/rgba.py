class RGBA(object):
    def __init__(self, r, g=None, b=None, a=255):
        def clamp(n, smallest, largest):
            return max(smallest, min(n, largest))

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

        return RGBA(r * 255, g * 255, b * 255, a * 255)

    def toTuple(self):
        return (self.rp(), self.gp(), self.bp())
