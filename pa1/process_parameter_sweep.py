import sys
import numpy as np
import matplotlib as mpl
mpl.use('Agg')
mpl.rcParams['text.usetex'] = True
import matplotlib.pyplot as plt
from scipy.interpolate import griddata

def parse_results(stream):
    results = dict([]);
    for line in stream:
        if line.startswith('=>'):
            _, name, val = line.strip().split()
            results[name] = results.get(name, []) + [float(val)]
    return results

def plot_by_params(test_name, results, dependent, plt_out_file):
    x = np.array(results['epsilon'])
    y = np.array(results['affect_rate'])

    X, Y = np.meshgrid(
        sorted(set(results['epsilon'])),
        sorted(set(results['affect_rate'])),
    )

    z = results[dependent]
    Z = griddata((x, y), z, (X, Y), method='linear')

    fig, ax = plt.subplots(1,1)
    ax.set_xticks([round(x_val, 2) for x_val in sorted(set(x))])
    ax.set_xlabel(r'$\varepsilon$', fontsize=20, labelpad=5)
    ax.set_yticks([round(y_val, 2) for y_val in sorted(set(y))])
    ax.set_ylabel(r'$\alpha$ (affect rate)', fontsize=16, labelpad=10)

    cp = ax.contourf(X, Y, Z)
    cb = fig.colorbar(cp)

    cb.set_label(r'\texttt{clock\textunderscore gettime} (seconds)', labelpad=20, fontsize=14, rotation=270)
    plt.title(r'AMR runtime for \texttt{' + test_name.replace('_', r'\textunderscore ') + '}', fontsize=16)

    plt.savefig(plt_out_file)

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} [test-name] [amr-output] [plot-file]')
        print(f'test-name : name of test')
        print(f'amr-output: (concatenation of) output file(s) from amr run')
        print(f'plt-file  : name of output image file for plot')
        exit()
    test_name    = sys.argv[1]
    amr_out_file = sys.argv[2]
    plt_out_file = sys.argv[3]

    with open(amr_out_file, 'r') as stream:
        results = parse_results(stream)

    plot_by_params(test_name, results, 'gettime-seconds', plt_out_file)
