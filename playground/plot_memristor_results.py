import matplotlib.pyplot as plt

xs = []
ys = []

for i in range(12):
    for j in range(2):
        x = []
        y = []
        with open(f'/home/alexander/Work/Data/memristor/test_data_1_{i + 1}_{j + 1}', 'r') as f:
            for line in f.readlines()[1:]:
                split = line.split(',')
                x.append(float(split[0].replace('\n', '')))
                y.append(float(split[1].replace('\n', '')))
        xs.append(x)
        ys.append(y)

for x, y in zip(xs, ys):
    plt.plot(x, y)

plt.show()
