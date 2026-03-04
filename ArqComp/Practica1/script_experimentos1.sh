#!/bin/bash
#SBATCH -n 1 -c 1 -t 00:10:00 --mem=4G
#SBATCH --job-name=acp1_mem

# Compilación
gcc acp1.c -o acp1 -O0 -std=gnu17

# Eliminar CSV anterior
rm -f salida.csv

# Cabecera CSV
echo "D,R,L,S_media,Ciclos_por_bucle,Ciclos_por_acceso" >> salida.csv

# Valores S1 e S2 xa calculados
S1=768
S2=20480

# Valores de L pedidos
L_values=(384 1152 10240 15360 40960 81920 163840)

# Valores de D (potencias de 2)
D_values=(1 8 64 512 4096)

# Executar experimento 10 veces
for rep in {1..10}
do
    for D in "${D_values[@]}"
    do
        for L in "${L_values[@]}"
        do
            ./acp1 $D $L >> salida.csv
        done
    done
done