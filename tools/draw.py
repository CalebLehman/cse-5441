import matplotlib as mpl
mpl.use('Agg')
mpl.rcParams['text.usetex'] = True
import matplotlib.pyplot as plt

fig = plt.figure()
ax = fig.add_subplot(111)
rect1 = mpl.patches.Rectangle((-200,-100), 400, 200, color='yellow', edgecolor='black')
rect2 = mpl.patches.Rectangle((0,150), 300, 20, color='red')
rect3 = mpl.patches.Rectangle((-300,-50), 40, 200, color='#0099FF')
circle1 = mpl.patches.Circle((-200,-250), radius=90, color='#EB70AA')
ax.add_patch(rect1)
ax.add_patch(rect2)
ax.add_patch(rect3)
ax.add_patch(circle1)
plt.xlim([-400, 400])
plt.ylim([-400, 400])
plt.savefig('foo.png')
