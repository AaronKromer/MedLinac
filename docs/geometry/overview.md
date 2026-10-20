# Geometry Overview

This document gives a high-level description of the simulation geometry used by the MedLinac example. The geometry is organized into a world (experimental hall), the treatment head and beam line elements, and a voxelized water phantom used as the sensitive volume for dose scoring and analysis.

- World / Experimental Hall:
  - A large box acting as the world volume; it contains all other components and uses `Air` as material. Visualization attributes for the world are normally turned off.

- Treatment Head and Beamline:
  - The head assembly contains collimators (jaws), target/filters, and optional MLC/beam modifiers implemented as modular components. Materials such as tungsten (W) are used for collimators.
  - Each component is constructed as a logical/physical volume and placed relative to the beam axis (z). Rotation matrices and placements are used for realistic positioning.

- Vacuum / Beam Pipe:
  - A vacuum box or pipe is included in front of the phantom to represent the beam path and reduce interactions before the phantom.

- Phantom (Voxelized Water Block):
  - The phantom is a water box partitioned into a 3D grid of voxels. Replication or parameterisation is used to create the voxel geometry efficiently.
  - The number of voxels and the total phantom dimensions are configurable. Voxel sizes are derived from phantom dimensions and the requested number of voxels per axis.
  - The voxel logical volume is typically set invisible in visualization to focus on the surrounding geometry.

- Sensitive Detector and Voxelisation:
  - The phantom voxels are created using a nested parameterisation (parameterised volumes) rather than a separate readout (RO) geometry. The parameterisation computes transforms and materials for each voxel efficiently and provides copy numbers used as voxel indices.
  - A sensitive detector (e.g. `MedLinacPhantomSD`) is attached to the voxel logical volume; it records energy depositions per voxel and forwards per-event data to the EventAction/Analysis components.

- Coordinate System and Indices:
  - The beam axis is the z direction. Replica/parameterisation copy numbers are used to obtain voxel indices in x, y and z; these indices are used for hit bookkeeping and mapping to physical positions.

- Performance and Implementation Notes:
  - Nested parameterisation or parameterised volumes are used where appropriate to minimize memory and construction cost.
  - For multi-threaded runs, per-thread or per-event buffering of hits is recommended; final aggregation should happen in the RunAction or via a thread-safe AnalysisManager.

- Visualization:
  - Visualization attributes are set on logical volumes to control visibility, color and rendering style. The phantom voxels are often hidden to declutter displays.

This overview is intentionally concise; for implementation details consult the detector construction and readout classes in the `src/` directory (for example, the readout geometry, sensitive detector, and parameterisation classes used to build the voxelized phantom).
