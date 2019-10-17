import sys

if __name__ == '__main__':
    results = dict([]);
    for line in sys.stdin:
        if line.startswith('=>'):
            _, name, val = line.strip().split()
            results[name] = results.get(name, []) + [float(val)]
    print(results)
