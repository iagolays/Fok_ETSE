#!/bin/bash
#SBATCH -n 1 -c 1 -t 00:10:00 --mem=4G
#SBATCH --job-name=acp2_mem

gcc acp2.c -o acp2 -O0 -std=gnu17

rm -f salida_int_ind.csv
echo "D,R,L,S_media,Ciclos_por_bucle,Ciclos_por_acceso" >> salida_int_ind.csv

L_values=(384 1152 10240 15360 40960 81920 163840)
D_values=(1 8 64 512 4096)

for rep in {1..10}
do
  for D in "${D_values[@]}"
  do
    for L in "${L_values[@]}"
    do
      ./acp2 "$D" "$L" >> salida_int_ind.csv
    done
  done
done