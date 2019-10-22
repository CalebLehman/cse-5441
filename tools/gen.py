import sys
from random import choice

class Box:
    def __init__(self, n=0, x=0, y=0, w=0, h=0, v=0):
        self.n = n
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.v = v

    def stringify(self):
        return f'({self.x}, {self.y})->({self.x+self.w}, {self.y+self.h})'

def flower_split(box, boxes):
    if box.w < 3 or box.h < 3:
        boxes.append(box)
        return []
    else:
        xs = [box.x, box.x + (box.w // 3), box.x + 2*(box.w // 3), box.x + box.w]
        ys = [box.y, box.y + (box.h // 3), box.y + 2*(box.h // 3), box.y + box.h]
        return [
            Box(0, xs[0], ys[0], xs[1]-xs[0], ys[1]-ys[0], 0),
            Box(0, xs[1], ys[0], xs[2]-xs[1], ys[1]-ys[0], 0),
            Box(0, xs[2], ys[0], xs[3]-xs[2], ys[1]-ys[0], 0),
            Box(0, xs[0], ys[1], xs[3]-xs[0], ys[2]-ys[1], 0),
            Box(0, xs[0], ys[2], xs[3]-xs[0], ys[3]-ys[2], 0),
        ]

def split_box(box, boxes):
    remaining = choice([
        flower_split,
    ])(box, boxes)
    for box in remaining:
        split_box(box, boxes)

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} [rows] [cols] [input-file]')
        print(f'rows:       number of rows')
        print(f'cols:       number of columns')
        print(f'input-file: path of output, which is an input file as described in assignments')
        exit()

    rows     = int(sys.argv[1])
    cols     = int(sys.argv[2])
    in_file  = sys.argv[3]

    box = Box(0, 0, 0, cols, rows, 0)
    boxes = []
    split_box(box, boxes)
    for box in boxes:
        print(box.stringify())
