//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: MedLinacElectronFoilsDecorator.hh,v 1.3 2006/06/29 16:04:07 gunter Exp $
//
// Code developed by: M. Piergentili
//
//


#ifndef MedLinacElectronFoilsDecorator_h
#define MedLinacElectronFoilsDecorator_h 1

#include "MedLinacDecorator.hh"
#include "MedLinacVGeometryComponent.hh"
class G4VPhysicalVolume;
class G4Box;
class G4LogicalVolume;
class G4Material;
class G4VisAttributes;
class MedLinacVGeometryComponent;
class MedLinacDecorator;

class MedLinacElectronFoilsDecorator: public MedLinacDecorator
{
public:
  MedLinacElectronFoilsDecorator(MedLinacVGeometryComponent*);
  ~MedLinacElectronFoilsDecorator();
  void ConstructComponent(G4VPhysicalVolume*,G4VPhysicalVolume*);
  void DestroyComponent(); 

private:
  void ConstructElectronFoils(G4VPhysicalVolume*,G4VPhysicalVolume*);
  G4LogicalVolume* side_log;
  G4LogicalVolume* sidelower_log;
  G4LogicalVolume* taDisk_log;
  G4LogicalVolume* alDisk_log;
  G4LogicalVolume* alDisk2_log;
  G4LogicalVolume* alDiskHolder_log;
  G4LogicalVolume* alDiskHolder2_log;
  G4LogicalVolume* alDiskHolder3_log;
  G4LogicalVolume* taDiskHolder_log;
  G4LogicalVolume* taDiskHolder2_log;
  G4LogicalVolume* taDiskHolder3_log;

  G4VPhysicalVolume* side_phys;
  G4VPhysicalVolume* sidelower_phys;
  G4VPhysicalVolume* taDisk_phys;
  G4VPhysicalVolume* alDisk_phys;
  G4VPhysicalVolume* alDisk2_phys;
  G4VPhysicalVolume* alDiskHolder_phys;
  G4VPhysicalVolume* alDiskHolder2_phys;
  G4VPhysicalVolume* alDiskHolder3_phys;
  G4VPhysicalVolume* taDiskHolder_phys;
  G4VPhysicalVolume* taDiskHolder2_phys;
  G4VPhysicalVolume* taDiskHolder3_phys;

  G4VisAttributes* simpleAlSVisAtt;
  G4VisAttributes* simpleTaSVisAtt;

  G4VisAttributes* simpleWorldVisAtt;
  //MedLinacVGeometryComponent* component;
};
#endif
