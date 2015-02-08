import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

def normalize_columns(s):
    return s.strip(' #')

df = pd.read_csv('results/ryan_petsc_ex5.csv', na_values='?', sep=',\s?').rename(columns=normalize_columns)

print(df)

pc_types = np.unique(df['pc_type'])

for pc_type in pc_types:
    plt.figure(1)
    ax = df[df['pc_type']==pc_type].plot('da_refine', 'avg_ksp_iterations', logy=True, label=pc_type)
    plt.figure(2)
    ax = df[df['pc_type']==pc_type].plot('da_refine', 'snes_solve_time_sec', logy=True, label=pc_type)

plt.figure(1)
plt.xlabel('da_refine')
plt.ylabel('Average KSP Iterations')
plt.legend(loc = 2)
plt.savefig('results/ryan_petsc_ex5_da-refine_vs_ksp-iters.png')
plt.figure(2)
plt.xlabel('da_refine')
plt.ylabel('SNES Solve Time (sec)')
plt.legend(loc = 2)
plt.savefig('results/ryan_petsc_ex5_da-refine_vs_snes-solve-time.png')

plt.show()
