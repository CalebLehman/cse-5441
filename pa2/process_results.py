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

def parse_results(stream):
    results = dict([]);
    for line in stream:
        if line.startswith('=>'):
            _, name, val = line.strip().split()
            results[name] = results.get(name, []) + [float(val)]
    return results

def plot_by_threads(results, dependent, plt_out_file):
    test_names = [test_name for test_name in results]
    programs = [program for program in results[test_names[0]]]

    scale = 7
    plt.figure(figsize=(scale*len(test_names), scale))
    lines = ['o-r', 'D-b', '+-g', 'x-r', 's-b']
    for t, test_name in enumerate(test_names):
        affect_rate = results[test_name][programs[0]]['affect_rate'][0]
        epsilon     = results[test_name][programs[0]]['epsilon'][0]
        ax = plt.subplot(1, len(test_names), t+1)
        for i, program in enumerate(programs):
            x = results[test_name][program]['threads']
            y = results[test_name][program][dependent]
            ax.plot(x, y, lines[i % len(lines)], label=program)
        ax.axhline(serial[test_name], color='g', ls='--', label='serial')
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
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} [plt-file] [results-dir] -t [test-names] -p [programs]')
        print(f'plt-file   : name of output image file for plot')
        print(f'results-dir: directory containing output file from testing script')
        exit()
    plt_out_file = sys.argv[1]
    results_dir  = sys.argv[2]

    results_files = os.listdir(results_dir)

    test_names = list(set(['_'.join(name.split('_')[:-2]) for name in results_files]))
    programs   = list(set([name.split('_')[-2] for name in results_files]))

    results = dict([])
    for test_name in test_names:
        results[test_name] = dict([])
        for program in programs:
            results_file = os.path.join(
                results_dir,
                f'{test_name}_{program}_results.txt'
            )
            with open(results_file, 'r') as stream:
                results[test_name][program] = parse_results(stream)

    plot_by_threads(results, 'gettime-seconds', plt_out_file)
