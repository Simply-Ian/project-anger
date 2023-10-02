#! /usr/bin/python3
w = int(input("Width: "))
h = int(input("Height: "))

print("[")
block = """{\"x\": %d, \"y\": %d},"""

for y in range(h):
    if y in (0, h - 1):
        for x in range(w): print(block % (x, y))
    else:
        print(block % (0, y))
        print(block % (w - 1, y))
print("],")