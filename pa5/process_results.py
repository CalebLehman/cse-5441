import sys, os
import numpy as np
import matplotlib as mpl
mpl.use('Agg')
mpl.rcParams['text.usetex'] = True
import matplotlib.pyplot as plt

serial = {
    'testgrid_400_12206': 261.09,
    'testgrid_1000_296793': 185.29,
}

def plot_by_threads(plt_out_file):
    scale = 7
    plt.figure(figsize=(scale * 2, scale))
    lines = ['o-r', 'o-b', 'x-r', 'x-b', 's-g']

    # 400
    program = 'lab5_mpi'
    test_name = 'testgrid_400_12206'
    results = [217, 202, 203, 207, 216, 215]
    threads = [1, 2, 10, 20, 30, 40]
    affect_rate = 0.01
    epsilon = 0.02

    ax = plt.subplot(1, 2, 1)
    ax.plot(threads, results, lines[0], label=program.replace('_', r'\textunderscore '))
    ax.axhline(261.09, color='g', ls='--', label='serial')
    ax.set_ylim((0, 400))

    ax.set_title(
          r'\texttt{' + test_name.replace('_', r'\textunderscore ') + r'}'
        + ' '
        + r'($\alpha=' + str(affect_rate) + '$'
        + r', '
        + r'$\varepsilon=' + str(epsilon) + '$)'
        , fontsize=20
    )
    ax.set_xlabel(r'\# of Threads', fontsize=18)
    ax.set_ylabel(r'Runtime (seconds)', fontsize=18)
    ax.legend()

    # 1000
    program = 'lab5_mpi'
    test_name = 'testgrid_1000_296793'
    results = [101, 83, 75, 74, 77, 78]
    threads = [1, 2, 10, 20, 30, 40]
    affect_rate = 0.9
    epsilon = 0.9

    ax = plt.subplot(1, 2, 2)
    ax.plot(threads, results, lines[0], label=program.replace('_', r'\textunderscore '))
    ax.axhline(185.29, color='g', ls='--', label='serial')
    ax.set_ylim((0, 300))

    ax.set_title(
          r'\texttt{' + test_name.replace('_', r'\textunderscore ') + r'}'
        + ' '
        + r'($\alpha=' + str(affect_rate) + '$'
        + r', '
        + r'$\varepsilon=' + str(epsilon) + '$)'
        , fontsize=20
    )
    ax.set_xlabel(r'\# of Threads', fontsize=18)
    ax.set_ylabel(r'Runtime (seconds)', fontsize=18)
    ax.legend()


    plt.tight_layout()
    plt.savefig(plt_out_file)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f'Usage: {sys.argv[0]} [plt-file]')
        print(f'plt-file   : name of output image file for plot')
        exit()
    plt_out_file = sys.argv[1]

    plot_by_threads(plt_out_file)
