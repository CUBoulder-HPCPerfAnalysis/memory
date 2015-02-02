#!/bin/bash

results_file="results/ryan_stencil.csv"

echo 'Dimensions (m), Cycles per DOF' | tee $results_file

max_pow2=9
m3=$((2**max_pow2));

for ((i=2; i<$((max_pow2+2)); i++)); do 
    m1=2
    m2=$((2**$i))
    for ((j=1; j<$((i-1)); j++)); do
        m1=$((m1*2))
        m2=$((m2/2))
        dim_string="$((m1))x$((m2))x$((m3))"
        if [ "$dim_string" == "512x4x512" ]; then continue; fi
        printf "$dim_string, " | tee -a $results_file
        ./stencil -m $m1 $m2 $m3 |
            grep "Cycles/DOF:" |
            awk '{ print $3 }' |
            tee -a $results_file
    done
done
