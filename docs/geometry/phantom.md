# Phantom and Voxelisation

This document describes how the water phantom is implemented using a nested (parameterised) voxelisation and how the sensitive detector is attached and used to record energy depositions.

Nested parameterisation
- The phantom is implemented as a mother logical volume filled with parameterised daughter voxels. A `MedLinacNestedPhantomParameterisation` (a `G4VPVParameterisation` subclass) computes the transform and optionally material for each voxel index along the z axis.
- Implementation pattern used in the code:
  - create a `G4LogicalVolume` that represents a single voxel (`logicalVolumeVoxel`).
  - create outer replica volumes for Y and X slices (or use nested parameterisation directly) and then a `G4PVParameterised` to place voxels along Z:

```cpp
// create logicalVolumeVoxel (single voxel)
G4LogicalVolume* logicalVolumeVoxel = new G4LogicalVolume(solidVoxel, H2O, "phantomVoxel");

// create the parameterisation and place parameterised voxels along Z
MedLinacNestedPhantomParameterisation* paramPhantom =
    new MedLinacNestedPhantomParameterisation(voxelHalfSize, nVoxelsZ, phantomMat);

new G4PVParameterised("PhantomSens", logicalVolumeVoxel, motherLogical, kUndefined,
                      nVoxelsZ, paramPhantom);
```

- The parameterisation returns the transform and the copy number for each placed voxel; the copy number is used as the voxel index along the parameterised axis. Replica/parameterisation copy numbers of nested levels give the x/y/z indices used by the sensitive detector.

Sensitive detector
- Attach the sensitive detector to the voxel logical volume (or to the parent replica logical volume if appropriate):

```cpp
G4LogicalVolume* lv = G4LogicalVolumeStore::GetInstance()->GetVolume("phantomVoxel");
if (lv) lv->SetSensitiveDetector(phantomSD);
```

Notes and recommendations
- Using nested parameterisation is efficient in memory and construction time compared to manually instantiating every voxel.
- For multi-threaded runs, avoid heavy locking in `ProcessHits()`; prefer buffering per-event or per-thread and do aggregation in `EndOfEventAction()` or `EndOfRunAction()`.
- Keep voxel logical volume invisible in visualization to reduce clutter.

See the detector construction (`src/MedLinacDetectorConstruction.cc`) and the parameterisation class (`src/MedLinacNestedPhantomParameterisation.cc`) for the exact implementation details.
