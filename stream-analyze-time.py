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
                   for f in glob.glob('results/fdkong-time.csv'))
print(df)

columns = ['Triad s','Add s','Dot s','Scale s', 'Copy s']
styles = ['bs-','ro-','y^-','bo-','d:']
linewidths = [1, 1, 1, 1,1,1]
fig, ax = pyplot.subplots()
ax.set_ylabel('Time(s)')
ax.set_xlabel('Block Size')
# ax.legend(lines, columns, loc='upper center')
for col, sty, lw in zip(columns, styles, linewidths):
    df.plot(x='Block Size', y=col, style=sty, lw=lw, ax=ax,title='Compute Time VS Block Size')
pyplot.legend(columns,loc='best')
pyplot.show()
# Does performance depend on block size?
# df.plot(x='Block Size', y='Triad s', kind='scatter')
# df.plot(x='Block Size', y='Dot s', kind='scatter')
# pyplot.show()

# Does performance depend on the number of threads used?
# df[df['machinename'].isin(['es.mcs.anl.gov'])].plot('CPU Cores used', 'Triad MB/s', kind='scatter')
# pyplot.show()
