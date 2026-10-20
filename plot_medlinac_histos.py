"""
Genera i grafici dagli istogrammi 1D contenuti in un file XML AIDA
(es. MedLinacDiane*.xml), come quelli prodotti da Geant4/AIDA con
histogram1d -> axis + data1d -> bin1d.

Uso:
    python plot_medlinac_histos.py percorso/al/file.xml [cartella_output]

Se non specifichi la cartella di output, i grafici vengono salvati
in una sottocartella "output" accanto al file XML.

Genera:
    - tutti_istogrammi.png   -> griglia 3x3 con tutti gli istogrammi
    - singoli/<id>_<titolo>.png -> un file per ogni istogramma
"""

import sys
import os
import xml.etree.ElementTree as ET

import numpy as np
import matplotlib.pyplot as plt


def load_histograms(xml_path):
    """
    Legge tutti gli <histogram1d> dal file XML AIDA e restituisce
    una lista di dict con: name, title, centers, heights, errors,
    entries, edges (bordi dei bin).
    """
    tree = ET.parse(xml_path)
    root = tree.getroot()

    histos = []
    for h in root.findall('histogram1d'):
        name = h.get('name')
        title = h.get('title')

        axis = h.find('axis')
        xmin = float(axis.get('min'))
        xmax = float(axis.get('max'))
        nbins = int(axis.get('numberOfBins'))

        heights = np.zeros(nbins)
        errors = np.zeros(nbins)
        entries = np.zeros(nbins)

        data = h.find('data1d')
        for b in data.findall('bin1d'):
            idx = int(b.get('binNum'))
            if idx < nbins:  # scarta l'eventuale bin underflow/overflow
                heights[idx] = float(b.get('height'))
                # error/entries potrebbero non essere sempre presenti
                if b.get('error') is not None:
                    errors[idx] = float(b.get('error'))
                if b.get('entries') is not None:
                    entries[idx] = float(b.get('entries'))

        edges = np.linspace(xmin, xmax, nbins + 1)
        centers = (edges[:-1] + edges[1:]) / 2

        histos.append(dict(
            name=name, title=title,
            centers=centers, heights=heights,
            errors=errors, entries=entries,
            edges=edges,
        ))

    return histos


def plot_grid(histos, out_path, ncols=3):
    """Griglia con tutti gli istogrammi insieme (colpo d'occhio d'insieme)."""
    nrows = int(np.ceil(len(histos) / ncols))
    fig, axes = plt.subplots(nrows, ncols, figsize=(5.3 * ncols, 4.3 * nrows))
    axes = np.atleast_1d(axes).flatten()

    for ax, hd in zip(axes, histos):
        ax.plot(hd['centers'], hd['heights'], color='#2563eb', linewidth=1.1)
        ax.fill_between(hd['centers'], hd['heights'], color='#2563eb', alpha=0.15)
        ax.set_title(f"{hd['title']} (id {hd['name']})", fontsize=11, fontweight='bold')
        ax.set_xlabel('posizione (mm)', fontsize=9)
        ax.set_ylabel('conteggi (height)', fontsize=9)
        ax.tick_params(labelsize=8)
        ax.grid(alpha=0.3)
        ax.set_xlim(hd['edges'][0], hd['edges'][-1])

    # nasconde eventuali assi vuoti se il numero di istogrammi non riempie la griglia
    for ax in axes[len(histos):]:
        ax.axis('off')

    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close(fig)


def plot_single(hd, out_path):
    """Un singolo grafico più grande e leggibile per un istogramma."""
    fig, ax = plt.subplots(figsize=(8, 5))
    ax.plot(hd['centers'], hd['heights'], color='#2563eb', linewidth=1.2)
    ax.fill_between(hd['centers'], hd['heights'], color='#2563eb', alpha=0.15)
    ax.set_title(f"{hd['title']} (id {hd['name']})", fontsize=13, fontweight='bold')
    ax.set_xlabel('posizione (mm)')
    ax.set_ylabel('conteggi (height)')
    ax.grid(alpha=0.3)
    ax.set_xlim(hd['edges'][0], hd['edges'][-1])
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close(fig)


def safe_filename(text):
    return text.replace(' ', '_').replace('/', '-')


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    xml_path = sys.argv[1]
    if len(sys.argv) >= 3:
        out_dir = sys.argv[2]
    else:
        out_dir = os.path.join(os.path.dirname(os.path.abspath(xml_path)), 'output')

    os.makedirs(out_dir, exist_ok=True)
    os.makedirs(os.path.join(out_dir, 'singoli'), exist_ok=True)

    histos = load_histograms(xml_path)
    if not histos:
        print("Nessun <histogram1d> trovato nel file.")
        sys.exit(1)

    plot_grid(histos, os.path.join(out_dir, 'tutti_istogrammi.png'))

    for hd in histos:
        fname = f"{hd['name']}_{safe_filename(hd['title'])}.png"
        plot_single(hd, os.path.join(out_dir, 'singoli', fname))

    print(f"Fatto. {len(histos)} istogrammi generati in: {out_dir}")
    print("  -", [h['title'] for h in histos])


if __name__ == '__main__':
    main()
