import sys
import matplotlib as mpl
mpl.use('Agg')
mpl.rcParams['text.usetex'] = True
import matplotlib.pyplot as plt

class Box:
    def __init__(self, n=0, x=0, y=0, w=0, h=0, v=0):
        self.n = n
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.v = v

def word_parser(in_file):
    with open(in_file, 'r') as stream:
        for line in stream:
            for word in line.strip().split():
                yield word

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} [input-file] [plot-file]')
        print(f'input-file: path to input file as described in assignments')
        print(f'plot-file:  path to output image file for plot')
        exit()

    in_file  = sys.argv[1]
    plt_file = sys.argv[2]

    parser = word_parser(in_file)
    N    = int(next(parser))
    rows = int(next(parser))
    cols = int(next(parser))
    boxes = [Box() for _ in range(N)]
    for box in boxes:
        box.id = int(next(parser))
        box.y  = int(next(parser))
        box.x  = int(next(parser))
        box.h  = int(next(parser))
        box.w  = int(next(parser))
        for _ in range(4):
            nhbrs = int(next(parser))
            for __ in range(nhbrs):
                next(parser)
        box.v = float(next(parser))

    max_v = max([box.v for box in boxes])
    min_v = min([box.v for box in boxes])

    fig = plt.figure()
    ax = fig.add_subplot(111)
    for box in boxes:
        ax.add_patch(
            mpl.patches.Rectangle(
                (box.x, box.y),
                box.w,
                box.h,
                facecolor=[0.4 + 0.6*(box.v - min_v)/(max_v - min_v), 0.1 + 0.9*(1-(box.v-min_v)/(max_v-min_v)), 0.1 + 0.9*(1-(box.v-min_v)/(max_v-min_v))],
                edgecolor='black'
            )
        )
    plt.xlim([0, cols])
    plt.ylim([0, rows])
    plt.savefig('foo.png')
