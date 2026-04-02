# stochastic-mas

## Benchmark auto (seq + MPI + speedup + plots)

Script: `benchmark.py`

### Prérequis
- `mpicc` + `mpirun` (OpenMPI)
- Python 3
- Optionnel pour les graphes: `matplotlib`

### Lancer en local
```bash
python3 benchmark.py --np 1,2,4,8 --repeats 3 --output-dir bench_out
```

### Résultats
- CSV: `bench_out/benchmark_results.csv`
- Graphes: `bench_out/runtime_vs_np.png` et `bench_out/speedup_vs_np.png`

### Sur GCP (VM)
```bash
sudo apt update
sudo apt install -y build-essential openmpi-bin libopenmpi-dev python3 python3-pip
pip3 install matplotlib
python3 benchmark.py --np 1,2,4,8,16 --repeats 3 --output-dir bench_out
```
