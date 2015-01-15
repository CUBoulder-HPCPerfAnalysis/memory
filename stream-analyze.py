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
                   for f in glob.glob('results/*.csv'))
print(df)

# Does performance depend on CPU clock speed?
df.plot(x='CPU GHz', y='Triad MB/s', kind='scatter')
pyplot.show()

# Does performance depend on the number of threads used?
df[df['machinename'].isin(['es.mcs.anl.gov'])].plot('CPU Cores used', 'Triad MB/s', kind='scatter')
pyplot.show()
