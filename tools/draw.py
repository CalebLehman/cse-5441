import sys
import matplotlib as mpl
mpl.use('Agg')
mpl.rcParams['text.usetex'] = True
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from collections import namedtuple

Box = namedtuple('Box', 'id y x h w v')

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
        for _ in range(4):
            nhbrs = int(next(parser))
            for __ in range(nhbrs):
                next(parser)
        v = float(next(parser))
        boxes[n] = Box(i, y, x, h, w, v)
    return boxes, rows, cols

def plot(plt_file, boxes, rows, cols):
    max_v = max([box.v for box in boxes])
    min_v = min([box.v for box in boxes])

    fig = plt.figure()
    ax = fig.add_subplot(111)
    for box in boxes:
        p = (box.v - min_v)/(max_v - min_v)
        ax.add_patch(
            mpl.patches.Rectangle(
                (box.x, box.y),
                box.w,
                box.h,
                facecolor=[
                    1.0*p + 1.0*(1-p),
                    0.0*p + 1.0*(1-p),
                    0.0*p + 0.0*(1-p),
                ],
                edgecolor='black'
            )
        )
    plt.xlim([0, cols])
    plt.ylim([0, rows])
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.yaxis.set_major_locator(MaxNLocator(integer=True))
    ax.invert_yaxis()
    ax.xaxis.tick_top()
    plt.savefig('foo.png')

def main():
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} [input-file] [plot-file]')
        print(f'input-file: path to input file as described in assignments')
        print(f'plot-file:  path to output image file for plot')
        exit()

    in_file  = sys.argv[1]
    plt_file = sys.argv[2]


    boxes, rows, cols = parse(in_file)
    plot(plt_file, boxes, rows, cols)

if __name__ == '__main__':
    main()
