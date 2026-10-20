#include <G4SystemOfUnits.hh>
#include "MedLinacElectronFoilsDecorator.hh"
#include "MedLinacDecorator.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4VPhysicalVolume.hh"

MedLinacElectronFoilsDecorator::MedLinacElectronFoilsDecorator(MedLinacVGeometryComponent* comp)
  : MedLinacDecorator(comp)
{
}

MedLinacElectronFoilsDecorator::~MedLinacElectronFoilsDecorator()
{
}

void MedLinacElectronFoilsDecorator::ConstructComponent(G4VPhysicalVolume* world,
                                                         G4VPhysicalVolume* vacuumBlock)
{
  MedLinacDecorator::ConstructComponent(world,vacuumBlock);
  ConstructElectronFoils(world,vacuumBlock);
}

void MedLinacElectronFoilsDecorator::DestroyComponent()
{
}

void MedLinacElectronFoilsDecorator::ConstructElectronFoils(G4VPhysicalVolume* world,
                                                             G4VPhysicalVolume* vacuumBlock)
{
  G4Material* Ta = new G4Material("Tantalum", 73., 180.95*g/mole, 16.65*g/cm3);
  G4Material* Al = new G4Material("Aluminum", 13., 26.98*g/mole, 2.70*g/cm3);

  const G4double startAngle = 0.*deg;
  const G4double spanningAngle = 360.*deg;
  G4RotationMatrix* rotateMatrix = new G4RotationMatrix();
  rotateMatrix->rotateX(180.*deg);

  G4Tubs* side = new G4Tubs("side", 2.575*cm, 3.375*cm, 1.02*cm, startAngle, spanningAngle);
  G4Tubs* sidelower = new G4Tubs("sidelower", 2.575*cm, 6.625*cm, 0.054*cm, startAngle, spanningAngle);
  G4Tubs* alDiskHolder = new G4Tubs("alDiskHolder", 0.4*cm, 3.475*cm, 0.004*cm, startAngle, spanningAngle);
  G4Tubs* alDiskHolder2 = new G4Tubs("alDiskHolder2", 0.*cm, 2.225*cm, 0.004*cm, startAngle, spanningAngle);
  G4Tubs* alDiskHolder3 = new G4Tubs("alDiskHolder3", 2.225*cm, 3.475*cm, 0.08*cm, startAngle, spanningAngle);
  G4Tubs* alDisk = new G4Tubs("alDisk", 0.*cm, 0.4*cm, 0.011*cm, startAngle, spanningAngle);
  G4Tubs* alDisk2 = new G4Tubs("alDisk2", 0.*cm, 0.499*cm, 0.043*cm, startAngle, spanningAngle);
  G4Tubs* taDisk = new G4Tubs("taDisk", 0.*cm, 0.635*cm, 0.00255*cm, startAngle, spanningAngle);
  G4Tubs* taDiskHolder = new G4Tubs("taDiskHolder", 2.2*cm, 3.2*cm, 0.075*cm, startAngle, spanningAngle);
  G4Tubs* taDiskHolder2 = new G4Tubs("taDiskHolder2", 0.635*cm, 1.575*cm, 0.075*cm, startAngle, spanningAngle);
  G4Cons* taDiskHolder3 = new G4Cons("taDiskHolder3", 1.425*cm, 1.575*cm, 2.2*cm, 2.35*cm,
                                     0.52*cm, startAngle, spanningAngle);

  side_log = new G4LogicalVolume(side, Al, "side_log");
  sidelower_log = new G4LogicalVolume(sidelower, Al, "sidelower_log");
  alDiskHolder_log = new G4LogicalVolume(alDiskHolder, Al, "alDiskHolder_log");
  alDiskHolder2_log = new G4LogicalVolume(alDiskHolder2, Al, "alDiskHolder2_log");
  alDiskHolder3_log = new G4LogicalVolume(alDiskHolder3, Al, "alDiskHolder3_log");
  alDisk_log = new G4LogicalVolume(alDisk, Al, "alDisk_log");
  alDisk2_log = new G4LogicalVolume(alDisk2, Al, "alDisk2_log");
  taDisk_log = new G4LogicalVolume(taDisk, Ta, "taDisk_log");
  taDiskHolder_log = new G4LogicalVolume(taDiskHolder, Al, "taDiskHolder_log");
  taDiskHolder2_log = new G4LogicalVolume(taDiskHolder2, Al, "taDiskHolder2_log");
  taDiskHolder3_log = new G4LogicalVolume(taDiskHolder3, Al, "taDiskHolder3_log");


  const G4double startingDistance = 102.4375*cm;
  G4ThreeVector position= G4ThreeVector(0.0*m, 0.0*m, startingDistance);

  sidelower_phys = new G4PVPlacement(rotateMatrix, position, "sidelower", sidelower_log, world, false, 0);

  position.setZ(startingDistance + 0.054*cm + 1.02*cm);
  side_phys = new G4PVPlacement(rotateMatrix, position, "side", side_log, world, false, 0);

  position.setZ(startingDistance - 0.054*cm - 0.004*cm);
  alDiskHolder_phys = new G4PVPlacement(rotateMatrix, position, "alDiskHolder", alDiskHolder_log, world, false, 0);

  position.setZ(startingDistance - 0.054*cm - 0.008*cm - 0.086*cm - 0.004*cm);
  alDiskHolder2_phys = new G4PVPlacement(rotateMatrix, position, "alDiskHolder2", alDiskHolder2_log, world, false, 0);
  
  position.setZ(startingDistance - 0.054*cm - 0.008*cm - 0.086*cm - 0.008*cm - 0.08*cm);
  alDiskHolder3_phys = new G4PVPlacement(rotateMatrix, position, "alDiskHolder3", alDiskHolder3_log, world, false, 0);
  
  position.setZ(startingDistance - 0.054*cm + 0.011*cm);
  alDisk_phys = new G4PVPlacement(rotateMatrix, position, "alDisk", alDisk_log, world, false, 0);

  position.setZ(startingDistance - 0.054*cm - 0.008*cm - 0.043*cm);
  alDisk2_phys = new G4PVPlacement(rotateMatrix, position, "alDisk2", alDisk2_log, world, false, 0);

  position.setZ(startingDistance + 0.054*cm + 2.04*cm + 0.075*cm);
  taDiskHolder_phys = new G4PVPlacement(rotateMatrix, position, "taDiskHolder", taDiskHolder_log, world, false, 0);
  
  position.setZ(startingDistance + 0.054*cm + 2.04*cm + 0.15*cm + 0.52*cm);
  taDiskHolder3_phys = new G4PVPlacement(rotateMatrix, position, "taDiskHolder3", taDiskHolder3_log, world, false, 0);

  position.setZ(startingDistance + 0.054*cm + 2.04*cm + 0.15*cm + 1.04*cm + 0.075*cm);
  taDiskHolder2_phys = new G4PVPlacement(rotateMatrix, position, "taDiskHolder2", taDiskHolder2_log, world, false, 0);
  
  position.setZ(startingDistance + 0.054*cm + 2.04*cm + 0.15*cm + 1.04*cm + 0.15*cm + 0.00255*cm);
  taDisk_phys = new G4PVPlacement(rotateMatrix, position, "taDisk", taDisk_log, world, false, 0);
}