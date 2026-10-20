# Medical Linac MT — Installation Guide

Geant4 multithreading port of the `medical_linac` simulation for intensity modulated radiation therapy (IMRT).

***

## Prerequisites

Before building, ensure the following are available on the system:

- **Geant4 11.4.1** (or compatible version)
- **iAIDA** library (for histogram output)
- **CMake** 
- **C++ compiler** 

### Configure Geant4 Environment

Source the Geant4 setup script before any build step:

```bash
. /usr/local/geant4/geant4.11.4.1-install/bin/geant4.sh
```

Add this line to your shell profile (`.bashrc`, `.zshrc`, etc.) to make it persistent.

***

## Build & Install

### 1. Clone the Repository

```bash
cd ~
git clone https://baltig.infn.it/skinscan/medlinacapps.git
cd medlinacapps/medical_linacMT
```

### 2. Build and Install iAIDA

iAIDA is required for the analysis/histogram output. Build it first:

```bash
cd iAIDA
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

By default, CMake assumes a standard system installation for iAIDA. If installed in a non-standard location, override the CMake variables at configure time:

```bash
cmake -DIAIDA_INCLUDE_DIR=/path/to/include \
      -DIAIDA_LIBRARY=/path/to/lib/libiAIDA.so ..
```

### 3. Build the Medical Linac Application

From `medlinacapps/medical_linacMT`:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

The `MedLinac` executable will be produced in the `build/` directory.

***

## Multithreading Configuration

The application supports runtime selection of the number of threads via a command-line argument.

### Usage

```
MedLinac [macro.mac] [nThreads]
```

| `nThreads` value | Behaviour |
|---|---|
| omitted | Single thread (safe default) |
| `0` | Automatic — uses all available CPU cores (`G4GetNumberOfCores()`) |
| `N > 0` | Uses exactly N threads |
| `N < 0` | Warning printed, falls back to single thread |

### Examples

```bash
# Single-threaded batch run
./MedLinac macro.mac

# 4-thread batch run
./MedLinac macro.mac 4

# Automatic multithreading (all cores)
./MedLinac macro.mac 0

# Interactive mode (single thread, opens Qt UI)
./MedLinac
```

### Implementation Notes

The thread count is resolved in `main()` before the run manager is initialised:

```cpp
G4int nThreads = 1;
auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

if (argc > 2) {
    nThreads = G4UIcommand::ConvertToInt(argv[2]);
    if (nThreads < 0) {
        G4cerr << "Warning: nThreads < 0, using single thread." << G4endl;
        nThreads = 1;
    } else if (nThreads == 0) {
        nThreads = G4Threading::G4GetNumberOfCores();
        G4cout << "Auto-threading: using all " << nThreads << " cores." << G4endl;
    }
}
runManager->SetNumberOfThreads(nThreads);
G4cout << "Running with " << nThreads << " thread(s)." << G4endl;
```

`SetNumberOfThreads()` must always be called **before** `runManager->Initialize()` (which is invoked by the macro, not explicitly in `main()`).

***

## Running the Simulation

### Batch Mode

```bash
# Without visualization
./MedLinac macro.mac

```

### Interactive Mode (Qt UI)

```bash
./MedLinac
```

***

## Interactive UI Commands

Once in interactive mode, the following UI commands are available.

### Physics

```
/PhysicsList/cut 0.2 mm
```

### Phantom & Detector

```
/Phantom/dimension 15. cm
/Phantom/Nvoxels 150
/Phantom/maxStep 0.2 mm

/PhantomSD/dimension 15. cm
/PhantomSD/Nvoxels 150
```

### Secondary Collimators (Jaws)

```
/Jaws/X1/DistanceFromAxis -20. cm
/Jaws/X2/DistanceFromAxis  20. cm
/Jaws/Y1/DistanceFromAxis -20. cm
/Jaws/Y2/DistanceFromAxis  20. cm
/Jaws/update
```

### Multi-Leaf Collimator (MLC)

Set each leaf position individually (distance from beam axis projected at isocenter):

```
/MLC/leaf_selection a1
/MLC/position 0. cm
/MLC/leaf_selection a2
/MLC/position 0. cm
...
/MLC/leaf_selection b39
/MLC/position 0. cm
/MLC/leaf_selection b40
/MLC/position 0. cm
```

### Beam Parameters

```
/energy 6.0 MeV
/sourceType 0.127 MeV
```

### Run

```
/run/beamOn [NumberOfEvents]
exit
```

> For reliable histogram statistics (histograms 4–8), use `NumberOfEvents > 500000`.

***

## Simulation Output

Results are written to `MedLinacDiane.xml` in the executable directory at the end of each run.


## Physics

The electromagnetic physics uses the Geant4 **LowEnergy** library, suited for the low-energy processes present in clinical IMRT scenarios. The default range cut is **0.1 mm**; a larger cut is applied to the first collimator.

***

## Geometry Overview

| # | Component |
|---|---|
| 1 | Point electron source (Gaussian energy and radial distribution, beam along Z) |
| 2 | Primary collimator (in vacuum box) |
| 3 | Target (in vacuum box) |
| 4 | Vacuum window |
| 5 | Flattening filter |
| 6 | Ion chamber |
| 7 | Mirror |
| 8 | Light field reticle |
| 9 | Secondary movable collimators (jaws) |
| 10 | Multi-Leaf Collimator (MLC) |
| 11 | Water phantom |

- SSD (source-to-surface distance): **100 cm**
- Particles exiting the target at angle > 25° are killed
- World volume filled with **air**