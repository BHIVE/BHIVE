import random

def move(prefered, at0, at1, at2, at3, at4 ):
    aList = [abs(at0-prefered),abs(at1-prefered),abs(at2-prefered),abs(at3-prefered),abs(at4-prefered)]
    return aList.index(max(aList))


