import pandas
import glob
from matplotlib import pyplot

df = pandas.concat(pandas.read_csv(f, na_values='?')
                   for f in glob.glob('results/ryan_stencil.csv'))
print(df)

ax = df.plot(kind='bar');
labels = df['Dimensions'];
ax.set_xticks(range(1,len(labels)));
ax.set_xticklabels(labels);
fig = ax.get_figure();
fig.savefig('results/ryan_Stencil-Dimensions_vs_Cycles-per-DOF.png');
pyplot.show()
