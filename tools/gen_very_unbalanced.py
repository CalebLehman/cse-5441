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

def get_nhbrs(box, ids, rows, cols):
    top = set([])
    if box.y > 0:
        for x in range(box.x, box.x+box.w):
            top.add(ids[box.y-1][x])

    bottom = set([])
    if box.y < rows-1:
        for x in range(box.x, box.x+box.w):
            bottom.add(ids[box.y+1][x])

    left = set([])
    if box.x > 0:
        for y in range(box.y, box.y+box.h):
            left.add(ids[y][box.x-1])

    right = set([])
    if box.x < cols-1:
        for y in range(box.y, box.y+box.h):
            right.add(ids[y][box.x+1])

    return [
        list(top),
        list(bottom),
        list(left),
        list(right),
    ]

def gen_boxes():
    cols  = 512
    rows  = 64+64*2
    boxes = []

    for y in range(64):
        for x in range(cols):
            boxes.append(Box(0, x, y, 1, 1, 0))
    for y in range(64, 64+64*2):
        if y % 2 == 0:
            for x in range(0, cols, cols):
                boxes.append(Box(0, x, y, cols, 1, 0))
        else:
            for x in range(cols):
                boxes.append(Box(0, x, y, 1, 1, 0))
    return (rows, cols, boxes)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f'Usage: {sys.argv[0]} [input-file]')
        print(f'input-file: path of output, which is an input file as described in assignments')
        exit()

    in_file = sys.argv[1]

    rows, cols, boxes = gen_boxes()
    boxes.sort(key=lambda b: (b.y, b.x))

    ids = [[-1 for _ in range(cols)] for _ in range(rows)]
    for i, box in enumerate(boxes):
        for y in range(box.y, box.y+box.h):
            for x in range(box.x, box.x+box.w):
                ids[y][x] = i
    boxes[len(boxes) // 2].v = 100

    with open(in_file, 'w') as out_file:
        out_file.write(f'{len(boxes)} {rows} {cols}\n')
        for i, box in enumerate(boxes):
            out_file.write('\n')
            out_file.write(f'{i}\n')
            out_file.write(f'{box.y} {box.x} {box.h} {box.w}\n')
            top, bottom, left, right = get_nhbrs(box, ids, rows, cols)
            out_file.write(f'{len(top)} {" ".join(map(str, top))}\n')
            out_file.write(f'{len(bottom)} {" ".join(map(str, bottom))}\n')
            out_file.write(f'{len(left)} {" ".join(map(str, left))}\n')
            out_file.write(f'{len(right)} {" ".join(map(str, right))}\n')
            out_file.write(f'{box.v}\n')
        out_file.write('\n')
        out_file.write('-1\n')
