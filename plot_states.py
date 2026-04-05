#!/usr/bin/env python3
import csv
import re
import subprocess
from pathlib import Path

import matplotlib.pyplot as plt


# --------------------
# Réglages simples
# --------------------
PROJECT_DIR = Path(".")
OUTPUT_DIR = PROJECT_DIR / "bench_out_states"

LINE_RE = re.compile(r"Jour\s+(\d+)\s+.*S=(\d+)\s+E=(\d+)\s+I=(\d+)\s+R=(\d+)")


def compile_codes():
    subprocess.run(["mpicc", "Simu_infct_seq.c", "-O2", "-lm", "-o", "sim_seq"], check=True)
    subprocess.run(["mpicc", "Simu_infct_mpi.c", "-O2", "-lm", "-o", "sim_mpi"], check=True)


def run_simulation(mode, np_val):
    if mode == "mpi":
        cmd = ["mpirun", "--use-hwthread-cpus", "-np", str(np_val), "./sim_mpi"]
    else:
        cmd = ["./sim_seq"]
    proc = subprocess.run(cmd, cwd=PROJECT_DIR, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if proc.returncode != 0:
        raise RuntimeError(proc.stderr)
    return proc.stdout


def parse_rows(text):
    rows = []
    for line in text.splitlines():
        m = LINE_RE.search(line)
        if m:
            rows.append(tuple(map(int, m.groups())))
    return rows


def generate_plot(mode, np_val):
    print(f"Génération pour {mode} (Cœurs: {np_val})...")
    out = run_simulation(mode, np_val)
    rows = parse_rows(out)

    csv_path = OUTPUT_DIR / f"states_{mode}_np{np_val}.csv"
    with csv_path.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["day", "S", "E", "I", "R"])
        for r in rows:
            w.writerow(r)

    day = [r[0] for r in rows]
    s = [r[1] for r in rows]
    e = [r[2] for r in rows]
    i = [r[3] for r in rows]
    rv = [r[4] for r in rows]

    plt.figure(figsize=(9, 5))
    plt.plot(day, s, label="Susceptibles (S)", color="blue")
    plt.plot(day, e, label="Exposés (E)", color="orange")
    plt.plot(day, i, label="Infectés (I)", color="red")
    plt.plot(day, rv, label="Rétablis (R)", color="green")
    plt.xlabel("Jour")
    plt.ylabel("Nombre d'agents")
    plt.title(f"Évolution de la pandémie SEIR ({mode.upper()}, {np_val} Cœurs) - 200k agents")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    png_path = OUTPUT_DIR / f"states_{mode}_np{np_val}.png"
    plt.savefig(png_path, dpi=150)
    plt.close()

    print(f"-> Terminé : {png_path}")

def main():
    OUTPUT_DIR.mkdir(exist_ok=True)
    compile_codes()
    generate_plot("seq", 1)
    generate_plot("mpi", 8)

