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
    affect_rate = results[test_names[0]][programs[0]]['affect_rate']
    epsilon     = results[test_names[0]][programs[0]]['epsilon']

    plt.figure(figsize=(8*len(test_names), 8))
    lines = ['o-r', '^-b']
    for t, test_name in enumerate(test_names):
        ax = plt.subplot(1, len(test_names), t+1)
        for i, program in enumerate(programs):
            x = results[test_name][program]['threads']
            y = results[test_name][program][dependent]
            ax.plot(x, y, lines[i % len(lines)])
        ax.axhline(serial[test_name], color='g', ls='--')

    """
    ax.set_xticks([round(x_val, 2) for x_val in sorted(set(x))])
    ax.set_xlabel(r'$\varepsilon$', fontsize=20, labelpad=5)
    ax.set_yticks([round(y_val, 2) for y_val in sorted(set(y))])
    ax.set_ylabel(r'$\alpha$ (affect rate)', fontsize=16, labelpad=10)

    cb.set_label(r'\texttt{clock\textunderscore gettime} (seconds)', labelpad=20, fontsize=14, rotation=270)
    """
    plt.title(r'AMR runtime for \texttt{' + test_name.replace('_', r'\textunderscore ') + '}', fontsize=16)
    plt.savefig(plt_out_file)

if __name__ == '__main__':
    if len(sys.argv) < 7:
        print(f'Usage: {sys.argv[0]} [plt-file] [results-dir] -t [test-names] -p [programs]')
        print(f'plt-file   : name of output image file for plot')
        print(f'results-dir: directory containing output file from testing script')
        print(f'test-names : names of tests')
        print(f'programs   : one or more programs used to generate output')
        exit()
    plt_out_file = sys.argv[1]
    results_dir  = sys.argv[2]

    assert sys.argv[3] == '-t', 'bad parameters'
    test_names = []
    pos = 4
    while sys.argv[pos] != '-p':
        test_names.append(sys.argv[pos])
        pos += 1

    assert sys.argv[pos] == '-p', 'bad parameters'
    programs = sys.argv[pos+1:]

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
