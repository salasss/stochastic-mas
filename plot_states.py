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
MODE = "mpi"   # "mpi" ou "seq"
NP = 4          # ignoré si MODE="seq"
OUTPUT_DIR = PROJECT_DIR / "bench_out_states"

LINE_RE = re.compile(r"Jour\s+(\d+)\s+.*S=(\d+)\s+E=(\d+)\s+I=(\d+)\s+R=(\d+)")


def compile_code():
    if MODE == "mpi":
        cmd = ["mpicc", "Simu_infct_mpi.c", "-O2", "-lm", "-o", "sim_mpi"]
    else:
        cmd = ["mpicc", "Simu_infct_seq.c", "-O2", "-lm", "-o", "sim_seq"]
    proc = subprocess.run(cmd, cwd=PROJECT_DIR, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if proc.returncode != 0:
        raise RuntimeError(proc.stderr)


def run_simulation():
    if MODE == "mpi":
        cmd = ["mpirun", "-np", str(NP), "./sim_mpi"]
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
    if not rows:
        raise RuntimeError("erreur")
    return rows


def main():
    OUTPUT_DIR.mkdir(exist_ok=True)
    compile_code()
    out = run_simulation()
    rows = parse_rows(out)

    csv_path = OUTPUT_DIR / f"states_{MODE}_np{NP}.csv"
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
    plt.plot(day, s, label="S")
    plt.plot(day, e, label="E")
    plt.plot(day, i, label="I")
    plt.plot(day, rv, label="R")
    plt.xlabel("Jour")
    plt.ylabel("Nombre d'agents")
    plt.title(f"S/E/I/R dans le temps ({MODE}, np={NP})")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    png_path = OUTPUT_DIR / f"states_{MODE}_np{NP}.png"
    plt.savefig(png_path, dpi=150)
    plt.close()

    print(f"CSV: {csv_path}")
    print(f"PNG: {png_path}")


if __name__ == "__main__":
    main()
