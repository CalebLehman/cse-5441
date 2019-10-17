import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import griddata

def parse_results(stream):
    results = dict([]);
    for line in stream:
        if line.startswith('=>'):
            _, name, val = line.strip().split()
            results[name] = results.get(name, []) + [float(val)]
    return results

def plot_by_params(results, dependent):
    x = np.array(results['affect_rate'])
    y = np.array(results['epsilon'])

    X, Y = np.meshgrid(
        sorted(set(results['affect_rate'])),
        sorted(set(results['epsilon'])),
    )

    z = results[dependent]
    Z = griddata((x, y), z, (X, Y), method='linear')

    fig, ax = plt.subplots(1,1)
    cp = ax.contourf(X, Y, Z)
    fig.colorbar(cp)

    plt.show()
    #plt.savefig('foo.png')

if __name__ == '__main__':
    if len(sys.argv) == 1:
        results = parse_results(sys.stdin)
    else:
        with open(sys.argv[1]) as stream:
            results = parse_results(stream)

    plot_by_params(results, 'time-seconds')
