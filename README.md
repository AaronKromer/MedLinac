# MedLinac

MedLinac is a Geant4-based Monte Carlo simulation of a medical linear
accelerator. It models the main components of the accelerator head and beam
line, including the collimators, a multileaf collimator (MLC), and a voxelized
water phantom.

The simulation transports primary and secondary particles through the
geometry and records energy deposits in the phantom. It supports interactive
execution with Geant4 visualization, batch runs controlled by macro files, and
multithreaded execution.

Development took place in the INFN Baltig repository; this repository presents
the completed version of the project.

## Highlights

- **Multithreaded event simulation:** Geant4 worker threads process events in
	parallel, and run-level results are merged for analysis.
- **Modular accelerator geometry:** the head, target and filter, and MLC are
	assembled from dedicated components.
- **Voxel-based energy scoring:** a sensitive detector records energy deposits
	throughout the 3D water phantom.
- **Flexible execution:** run interactively with visualization or in batch
	mode using macros to configure the simulation.

## Requirements

- Geant4 (the installation guide is based on version 11.4.1)
- CMake 3.16 or later
- A compatible C++ compiler
- iAIDA for analysis and histogram output

Before configuring the project, load the Geant4 environment by sourcing the
`geant4.sh` script from your installation. CMake looks for iAIDA in the
standard `/usr/local` paths; if it is installed elsewhere, set
`IAIDA_INCLUDE_DIR` and `IAIDA_LIBRARY` during configuration.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The `MedLinac` executable is created in the `build/` directory.

## Run

To run a batch simulation, from the build directory:

```bash
cd build
./MedLinac macro.mac 4
```

The second argument specifies the number of threads. If omitted, the
application uses one thread; `0` requests automatic use of the available CPU
cores. To start the interactive mode with visualization, run `./MedLinac`
without arguments. Macro files and Geant4 commands can be used to configure
the source, geometry, phantom, and other simulation parameters.

## Geometry Confidentiality

The geometry values and dimensions in the source code and configuration files
have been changed for confidentiality reasons. Therefore, the simulated
geometry may not match the actual equipment, and the results should not be
treated as a description or validation of its real parameters. Please contact
me for further information.

## Documentation

- [Introduction and code overview](docs/Introduction.md)
- [Installation guide](docs/installation/installation.md)