#!/usr/bin/env python3
import csv
import statistics
import subprocess
import time
from pathlib import Path

import matplotlib.pyplot as plt


# --------------------
# Réglages simples
# --------------------
PROJECT_DIR = Path(".")
NP_LIST = [1, 2, 4, 8, 16]
REPEATS = 2
OUTPUT_DIR = PROJECT_DIR / "bench_out"


def run_and_time(cmd):
    start = time.perf_counter()
    proc = subprocess.run(cmd, cwd=PROJECT_DIR, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    elapsed = time.perf_counter() - start
    if proc.returncode != 0:
        raise RuntimeError(f"erruer")
    return elapsed


def compile_all():
    run_and_time(["mpicc", "Simu_infct_seq.c", "-O2", "-lm", "-o", "sim_seq"])
    run_and_time(["mpicc", "Simu_infct_mpi.c", "-O2", "-lm", "-o", "sim_mpi"])


def mean_std(values):
    if len(values) == 1: return values[0], 0.0
    return statistics.mean(values), statistics.stdev(values)


def main():
    OUTPUT_DIR.mkdir(exist_ok=True)

    print("Compil////////////")
    compile_all()

    print("Run seq////////////")
    seq_times = [run_and_time(["./sim_seq"]) for _ in range(REPEATS)]
    seq_mean, seq_std = mean_std(seq_times)

    print("Run MPI//////////////")
    mpi_means = []
    mpi_stds = []
    for np_val in NP_LIST:
        times = [run_and_time(["mpirun", "--oversubscribe", "-np", str(np_val), "./sim_mpi"]) for _ in range(REPEATS)]
        m, s = mean_std(times)
        mpi_means.append(m)
        mpi_stds.append(s)
        print(f"np={np_val}  mean={m:.3f}s  std={s:.3f}s  speedup={seq_mean/m:.2f}x")

    csv_path = OUTPUT_DIR / "benchmark_results.csv"
    with csv_path.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["mode", "np", "mean_s", "std_s", "speedup_vs_seq"])
        w.writerow(["seq", 1, f"{seq_mean:.6f}", f"{seq_std:.6f}", "1.000000"])
        for np_val, m, s in zip(NP_LIST, mpi_means, mpi_stds):
            w.writerow(["mpi", np_val, f"{m:.6f}", f"{s:.6f}", f"{seq_mean / m:.6f}"])

    # Plot runtime
    plt.figure(figsize=(7, 4))
    plt.plot(NP_LIST, mpi_means, marker="o")
    plt.xlabel("np")
    plt.ylabel("Temps moyen (s)")
    plt.title("Runtime MPI")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(OUTPUT_DIR / "runtime_vs_np.png", dpi=140)
    plt.close()

    # Plot speedup
    speedups = [seq_mean / m for m in mpi_means]
    plt.figure(figsize=(7, 4))
    plt.plot(NP_LIST, speedups, marker="o", label="Mesuré")
    plt.plot(NP_LIST, NP_LIST, "--", label="Idéal")
    plt.xlabel("np")
    plt.ylabel("Speedup")
    plt.title("Speedup MPI")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(OUTPUT_DIR / "speedup_vs_np.png", dpi=140)
    plt.close()

    print(f"tout est bon ////////// {csv_path}")


if __name__ == "__main__":
    main()
