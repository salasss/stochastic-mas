#!/usr/bin/env python3
import subprocess

def run_simulation(np):
    cmd = ["mpirun", "-np", str(np), "./sim_mpi_repro"]
    result = subprocess.run(cmd, stdout=subprocess.PIPE, text=True)
    
    # recup dernière ligne
    last_line = result.stdout.strip().split('\n')[-1]
    return last_line

print("Compil///////")
subprocess.run(["mpicc", "Simu_infct_mpi_repro.c", "-O2", "-lm", "-o", "sim_mpi_repro"])

print("\n--- TEST DE REPRODUCTIBILITÉ SELON LES CŒURS ---")
print("relance exactement la même simulation, avec la même seed\n")

for nb_cores in [1, 2, 4,8]:
    resultat = run_simulation(nb_cores)
    print(f"Avec {nb_cores} cœurs////////Résultat final : {resultat}")

print("\nfin////////")
