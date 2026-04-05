# Simulation SEIR MPI

Simulation multi-agents d'une pandémie (Modèle SEIR) en C, avec parallélisation MPI et version séquentielle optimisée.

## Prérequis
- OpenMPI (`mpicc`, `mpirun`)
- Python 3 + pip
- Matplotlib (pour la génération des courbes)

## Installation (Environnement)
```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## Utilisation (Scripts Python)

**1. Générer les courbes épidémiques S/E/I/R (200 000 agents)**
```bash
python plot_states.py
```
*(Les graphiques `states_*.png` seront dans `bench_out_states/`)*

**2. Lancer le Benchmark de performances (Temps / Speedup)**
```bash
python benchmark.py --np 1,2,4,8,16 --repeats 3 --output-dir bench_out
```

**3. Valider la reproductibilité (Seed aléatoire)**
```bash
python check_repro.py
```
