import pandas
import glob
from matplotlib import pyplot

def normalize_columns(s):
    """Fix column names"""
    s = s.strip(' #')
    try:
        return {'Machinename':'machinename', 'Username':'username'}[s]
    except:
        return s

df = pandas.concat(pandas.read_csv(f, na_values='?').rename(columns=normalize_columns)
                   for f in glob.glob('results/ryan_Add_BlkCyc.csv'))
print(df)

# Does performance depend on CPU clock speed?
ax = df.plot(x='Stream Skip Length', y='Best Rate MB/s', kind='scatter')
fig = ax.get_figure();
fig.savefig('results/ryan_Best_Rate_vs_Skip_Length.png');
pyplot.show()
