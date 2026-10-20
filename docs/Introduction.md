# Medical Linac MT: General Code Overview

This project is a Geant4 simulation of a medical linear accelerator. It models
the accelerator head, collimation system, multileaf collimator (MLC), and a
water phantom. The simulation can run in Geant4 multi-threaded mode. Worker
threads transport particles and collect detector information, while the master
thread merges the worker results and performs the final run-level analysis.

## 1. Program Entry Point

The application starts in `MedLinac.cc`, in the `main()` function.

The startup sequence is:

1. Select and seed the CLHEP random-number engine.
2. Create the Geant4 run manager with `G4RunManagerFactory`.
3. Read the requested number of threads from the command line.
	 - The default is one thread.
	 - A value of zero requests automatic use of all available processor cores.
4. Create the detector construction and register it with the run manager.
5. Register `MedLinacPhysicsList`.
6. Optionally create and initialize the visualization manager.
7. Register `MedLinacActionInitialization`.
8. Start an interactive UI session, or execute the macro supplied on the
	 command line in batch mode.

The program uses interactive mode when it is launched without arguments. In
batch mode, the first argument is the macro file and the second argument can
specify the number of threads. For example:

```text
./MedLinac macro.mac 8
```

The run manager initialization is normally triggered by the `/run/initialize`
command in the macro. This is important because geometry, particles, physics
processes, and user actions must be registered before the first event is
processed.

## 2. Detector Construction

`MedLinacDetectorConstruction` is responsible for creating the complete
simulation geometry. Its `Construct()` method delegates to `ConstructGeom()`.
The geometry is built around a beam line parallel to the z axis.

### World and beam-line volumes

The detector construction creates:

- An air-filled experimental hall, which is the world volume.
- A low-density vacuum block containing the main accelerator components.
- Four tungsten jaws: X1, X2, Y1, and Y2.
- A water phantom placed downstream of the accelerator head.

The jaw positions and rotations are calculated from the configured jaw field
values. The detector messenger exposes commands that can modify these values,
the phantom dimensions and distance, the voxel count, and the maximum step
length. `/Jaws/update` requests a geometry rebuild after jaw parameters have
been changed.

### Accelerator head and component decorators

The head is assembled in `MedLinacHead`. It contains the fixed accelerator-head
components placed in or around the vacuum block, such as windows, collimators,
the mirror, and the reticle.

Additional components are added through decorators:

- `MedLinacTargetAndFilterDecorator` adds the target and flattening-filter
	components. The target includes tungsten and copper parts, and the filter is
	constructed from conical or related solids.
- `MedLinacMLCDecorator` adds the multileaf collimator. It creates two banks of
	leaves, usually referred to as the A and B banks, and provides messenger
	commands for selecting and moving individual leaves.

The decorators wrap the head component and call the underlying component first.
`MedLinacDetectorConstruction::ConstructVolume()` therefore builds the head,
then the target and filter, and finally the MLC.

## 3. Water Phantom and Voxelization

The phantom is a water box divided into a three-dimensional voxel structure.
The hierarchy is:

1. The water phantom mother volume.
2. Y replicas (`YSlice`).
3. X replicas (`XSlice`) inside each Y slice.
4. Parameterized Z voxels (`PhantomSens`), using
	 `MedLinacNestedPhantomParameterisation`.

The number of voxels is configured by `/Phantom/Nvoxels`. The same value is
currently used along X, Y, and Z, so the phantom is divided into a cubic grid.
The phantom user limit can be set with `/Phantom/maxStep`.

The sensitive detector is created in `ConstructSDandField()`. The
`MedLinacPhantomSD` object is attached to the logical volume named
`phantomVoxel`. Each energy-depositing step in a voxel can consequently be
recorded by `MedLinacPhantomSD::ProcessHits()`.

## 4. Physics List

`MedLinacPhysicsList` derives from `G4VUserPhysicsList` and defines the
particles and electromagnetic processes used by the simulation.

### Particles

The physics list constructs:

- Photons (`gamma`)
- Electrons (`e-`)
- Positrons (`e+`)

### Processes

Transportation is added for all defined particles. The electromagnetic
processes are:

- Photons: photoelectric effect, Compton scattering, and gamma conversion.
- Electrons: multiple scattering, ionization, and bremsstrahlung.
- Positrons: multiple scattering, ionization, bremsstrahlung, and
	annihilation.

Production cuts can be configured through the physics-list messenger. The
macro uses `/PhysicsList/cut 0.2 mm` before initialization.

## 5. Primary Particle Generation

`MedLinacPrimaryGeneratorAction` is created for every worker thread. It uses a
`G4ParticleGun` to generate one primary electron per event.

The default primary source is configured as follows:

- Particle: electron.
- Position: approximately `(0, 0, 123 cm)`.
- Direction: mainly along the negative z direction, with a small angular
	spread generated from a Gaussian distribution.
- Energy: sampled from a Gaussian distribution using the configured mean and
	standard deviation.

The primary generator messenger provides runtime commands for changing source
parameters. The source produces the primary vertex in
`GeneratePrimaries(G4Event*)`; Geant4 then transports that primary and all
secondary particles through the accelerator geometry and phantom.

## 6. Multi-threaded Event Flow

The action classes are registered by `MedLinacActionInitialization`:

### Master thread

`BuildForMaster()` registers only `MedLinacRunAction`. The master does not
generate or transport events. It coordinates the run and receives merged run
data from the worker threads.

### Worker threads

`Build()` registers the actions that execute during event simulation:

- `MedLinacPrimaryGeneratorAction` creates the primary particle.
- `MedLinacRunAction` creates and manages a `MedLinacRun` object.
- `MedLinacTrackingAction` controls trajectory storage for visualization.
- `MedLinacEventAction` manages event-local hit data and event boundaries.

Geant4 distributes events among the worker threads. Each worker processes its
assigned events independently and maintains its own event and run data. This
avoids requiring the hot detector path to write to shared analysis objects.

## 7. Hit Collection and Run Merging

The hit-data flow is:

1. A particle deposits energy in a `phantomVoxel` volume.
2. `MedLinacPhantomSD::ProcessHits()` reads the voxel copy numbers from the
	 touchable history.
3. The copy numbers are converted into physical X, Y, and Z coordinates.
4. A `MedLinacHit` record is created containing the position and deposited
	 energy.
5. The hit is passed to the current worker's `MedLinacEventAction`.
6. At the end of the event, `MedLinacEventAction` appends the event hits to the
	 current worker's `MedLinacRun`.
7. At the end of the run, Geant4 calls `MedLinacRun::Merge()` to append each
	 worker run's hit collection to the master run.

The custom run class is used instead of writing directly to histograms from
`ProcessHits()`. Detector processing is called very frequently and may happen
concurrently on several workers, whereas the buffered run-level data can be
merged in a controlled way.

## 8. Analysis

When analysis support is enabled with `G4ANALYSIS_USE`, the master run action
books the histograms at the beginning of the run. At the end of the run it
iterates over the merged `MedLinacHit` records and calls
`MedLinacRunAction::AnalyzeHit()`.

The current analysis includes energy-deposition distributions for:

- Percentage depth dose (PDD) along the phantom depth axis.
- Beam flatness along X at several depths.
- Beam flatness along Y at several depths.

The analysis applies optional central-chamber cuts and fills histograms only
for hits with nonzero deposited energy. The analysis manager finalizes and
writes the output when the master run ends.

## 9. Macro Execution Order

`macro.mac` is an example batch configuration. Its general order is:

1. Set physics and run verbosity.
2. Configure phantom dimensions, distance, voxel count, and maximum step.
3. Configure the MLC leaf positions.
4. Configure jaw positions and issue `/Jaws/update`.
5. Execute `/run/initialize` so the final geometry is built.
6. Configure the beam energy and source parameters.
7. Start the simulation with `/run/beamOn`.

Commands that change geometry should be issued before the final initialization,
or followed by the appropriate geometry update and reinitialization. The
number of events is controlled by `/run/beamOn`.

## 10. Main Source Locations

- `MedLinac.cc`: application entry point, run manager, threads, UI, and macro
	execution.
- `src/MedLinacDetectorConstruction.cc`: world, jaws, phantom, and sensitive
	detector attachment.
- `src/MedLinacHead.cc`: fixed accelerator-head geometry.
- `src/MedLinacTargetAndFilterDecorator.cc`: target and flattening filter.
- `src/MedLinacMLCDecorator.cc`: multileaf collimator.
- `src/MedLinacPhysicsList.cc`: particles, transportation, and electromagnetic
	processes.
- `src/MedLinacPrimaryGeneratorAction.cc`: primary electron source.
- `src/MedLinacPhantomSD.cc`: energy-deposition hit extraction.
- `src/MedLinacEventAction.cc`: event-local hit buffering.
- `src/MedLinacRun.cc`: worker-run storage and master merging.
- `src/MedLinacRunAction.cc`: run lifecycle and final histogram analysis.
- `src/MedLinacTrackingAction.cc`: trajectory-storage decisions for
	visualization.