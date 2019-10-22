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

def flower_split(box, boxes):
    if box.w < 3 or box.h < 3:
        boxes.append(box)
        return []
    else:
        xs = [box.x, box.x + (box.w // 3), box.x + 2*(box.w // 3), box.x + box.w]
        ys = [box.y, box.y + (box.h // 3), box.y + 2*(box.h // 3), box.y + box.h]
        return [
            Box(0, xs[0], ys[0], xs[1]-xs[0], ys[2]-ys[0], 0),
            Box(0, xs[1], ys[0], xs[3]-xs[1], ys[1]-ys[0], 0),
            Box(0, xs[1], ys[1], xs[2]-xs[1], ys[2]-ys[1], 0),
            Box(0, xs[2], ys[1], xs[3]-xs[2], ys[3]-ys[1], 0),
            Box(0, xs[0], ys[2], xs[2]-xs[0], ys[3]-ys[2], 0),
        ]

def vertical_split(box, boxes):
    if box.w < 2:
        boxes.append(box)
        return []
    else:
        return [
            Box(0, box.x, box.y, box.w // 2, box.h, 0),
            Box(0, box.x + box.w // 2, box.y, box.w - box.w // 2, box.h, 0),
        ]

def horizontal_split(box, boxes):
    if box.h < 2:
        boxes.append(box)
        return []
    else:
        return [
            Box(0, box.x, box.y, box.w, box.h // 2, 0),
            Box(0, box.x, box.y + box.h // 2, box.w, box.h - box.h // 2, 0),
        ]

def quarter_split(box, boxes):
    if box.h < 2 or box.w < 2:
        boxes.append(box)
        return []
    else:
        return [
            Box(0, box.x, box.y, box.w // 2, box.h // 2, 0),
            Box(0, box.x + box.w // 2, box.y, box.w - box.w // 2, box.h // 2, 0),
            Box(0, box.x, box.y + box.h // 2, box.w // 2, box.h - box.h // 2, 0),
            Box(0, box.x + box.w // 2, box.y + box.h // 2, box.w - box.w // 2, box.h - box.h // 2, 0),
        ]

def split_box(box, boxes):
    remaining = choice([
        flower_split,
        vertical_split,
        horizontal_split,
        quarter_split,
    ])(box, boxes)
    for box in remaining:
        split_box(box, boxes)

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
            left.add(ids[y][box.x+1])

    return [
        list(top),
        list(bottom),
        list(left),
        list(right),
    ]

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} [rows] [cols] [input-file]')
        print(f'rows:       number of rows')
        print(f'cols:       number of columns')
        print(f'input-file: path of output, which is an input file as described in assignments')
        exit()

    rows    = int(sys.argv[1])
    cols    = int(sys.argv[2])
    in_file = sys.argv[3]

    box = Box(0, 0, 0, cols, rows, 0)
    boxes = []
    split_box(box, boxes)
    boxes.sort(key=lambda b: (b.y, b.x))

    ids = [[-1 for _ in range(cols)] for _ in range(rows)]
    for i, box in enumerate(boxes):
        for y in range(box.y, box.y+box.h):
            for x in range(box.x, box.x+box.w):
                ids[y][x] = i
    choice(boxes).v = 100

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
