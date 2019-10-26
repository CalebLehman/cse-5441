import sys
import numpy as np
from collections import namedtuple

Box = namedtuple('Box', 'id y x h w v nhbrs')

def parse(in_file):
    def input_parser(in_file):
        with open(in_file, 'r') as stream:
            for line in stream:
                for word in line.split('#')[0].strip().split():
                    yield word
    parser = input_parser(in_file)

    N    = int(next(parser))
    rows = int(next(parser))
    cols = int(next(parser))
    boxes = [None for _ in range(N)]
    for n, box in enumerate(boxes):
        i = int(next(parser))
        y = int(next(parser))
        x = int(next(parser))
        h = int(next(parser))
        w = int(next(parser))
        nhbrs = 0
        for _ in range(4):
            curr_nhbrs = int(next(parser))
            nhbrs += curr_nhbrs
            for __ in range(curr_nhbrs):
                next(parser)
        v = float(next(parser))
        boxes[n] = Box(i, y, x, h, w, v, nhbrs)
    return boxes, rows, cols

def stats(boxes, rows, cols):
    nhbrs = np.array([box.nhbrs for box in boxes])
    return {
        'rows'     : rows,
        'cols'     : cols,
        'num'      : len(boxes),
        'avg_nhbrs': np.mean(nhbrs),
        'std_nhbrs': np.std(nhbrs),
    }

def main():
    if len(sys.argv) != 2:
        print(f'Usage: {sys.argv[0]} [input-file]')
        print(f'input-file: path to input file as described in assignments')
        exit()

    in_file  = sys.argv[1]

    boxes, rows, cols = parse(in_file)
    print(stats(boxes, rows, cols))

if __name__ == '__main__':
    main()
