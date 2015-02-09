#!/bin/bash

# This script runs various configurations of PETSc's example 5, dealing with the Bratu
#   problem, and parses output to a file for further post-processing.
#
# *** Run this script from within the 'memory' directory. ***
#
# Ryan Skinner 2015-02-06

num_procs=2
results_file=${PWD}/results/ryan_petsc_ex5.csv

cd ${PETSC_DIR?"Environment variable PETSC_DIR is not set, and needs to be."}
cd src/snes/examples/tutorials/

if ! [ -x ex5 ]; then
    echo ===================================================
    echo Example 5 has not been compiled yet. Compiling now.
    echo ===================================================
    make ex5
    echo ===================================================
    echo Done compiling.
    echo ===================================================
fi

echo 'num_procs, pc_type, da_refine, avg_ksp_iterations, snes_solve_time_sec' | tee $results_file

for pc_type in bjacobi asm gamg mg; do
    for da_refine in {1..8}; do
        mpiexec -n $num_procs ./ex5 -ksp_converged_reason -log_summary -pc_type $pc_type -da_refine $da_refine |
            grep './ex5 on a\|Linear solve converged due to\|Time (sec):' |
            awk -v pc_type="$pc_type" -v da_refine="$da_refine" '
                BEGIN {
                    n_ksp_solves=0;
                    tot_ksp_iters=0;
                }
                {
                    if (match($0,"[0-9]+ processors")) {
                        num_procs = substr($0, RSTART, RLENGTH-length(" processors"));
                    }
                    if ($1 == "Linear") {
                        n_ksp_solves++;
                        tot_ksp_iters += $8
                    }
                    if ($1 == "Time") {
                        avg_ksp_iters = tot_ksp_iters / n_ksp_solves;
                        snes_solve_time = $5;
                        printf("%i, %s, %i, %f, %f\n", num_procs, pc_type, da_refine, avg_ksp_iters, snes_solve_time);
                    }
                }' |
            tee -a $results_file
    done
done
