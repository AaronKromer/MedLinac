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
// $Id: MedLinacDetectorConstruction.cc,v 1.12 2007/07/06 21:39:05 mpiergen Exp $
//
// Code developed by: M. Piergentili
//
//
//------------------------------ beam line along z axis---------------------
#include <G4SystemOfUnits.hh>
#include "MedLinacDetectorMessenger.hh"
#include "MedLinacDetectorConstruction.hh"
#include "MedLinacPhantomROGeometry.hh"
#include "MedLinacPhantomSD.hh"
#include "MedLinacVGeometryComponent.hh"
#include "MedLinacHead.hh"
#include "MedLinacDecorator.hh"
#include "MedLinacTargetAndFilterDecorator.hh"
#include "MedLinacDummySD.hh"
#include "MedLinacDummySD.hh"
#include "MedLinacNestedPhantomParameterisation.hh"
#include "MedLinacElectronFoilsDecorator.hh"
#include "MedLinacMLCDecorator.hh"
#include "G4CSGSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4MaterialTable.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4TransportationManager.hh"
#include "G4UserLimits.hh"
#include "G4GeometryManager.hh"
#include "G4BooleanSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4VSolid.hh"
#include "G4Region.hh"

#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RegionStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4NistManager.hh"

MedLinacDetectorConstruction::MedLinacDetectorConstruction(G4String SDName)
  : phantomSD(0),phantomROGeometry(0),
    experimentalHall_log(0),vacuumBlock_log(0),
    JawY1_log(0),JawY2_log(0),
    JawX1_log(0),JawX2_log(0),
    Phantom_log(0),
    experimentalHall_phys(0),vacuumBlock_phys(0),
    JawY1_phys(0), JawY2_phys(0),
    JawX1_phys(0), JawX2_phys(0),
    Phantom_phys(0)
{
  phantomDim = 15.*cm;
   //PhantomDimensionX = PhantomDim_x;
   //PhantomDimensionY = PhantomDim_y;
   //PhantomDimensionZ = PhantomDim_z;

  //numberOfVoxelsAlongX=150;
  //numberOfVoxelsAlongY=150;
  //numberOfVoxelsAlongZ=150;

  numberOfVoxelsAlongX=0;
  numberOfVoxelsAlongY=0;
  numberOfVoxelsAlongZ=0;
  maxStep = 0;

  sensitiveDetectorName = SDName;

 // default parameter values

  fieldX1 = -2.5*cm;
  fieldX2 =  2.5*cm;
  fieldY1 = -2.5*cm;
  fieldY2 =  2.5*cm;


  detectorMessenger = new MedLinacDetectorMessenger(this);
  pHead = new MedLinacHead();
  MLCDecorator = new MedLinacMLCDecorator(pHead);
  if (electronMode){
    filterDecorator = new MedLinacElectronFoilsDecorator(pHead);
  }
  else{
    filterDecorator = new MedLinacTargetAndFilterDecorator(pHead);
  }
  ComputeDimVoxel();


  }

MedLinacDetectorConstruction* MedLinacDetectorConstruction::instance = 0;

MedLinacDetectorConstruction* MedLinacDetectorConstruction::GetInstance(G4String SDName)
{
  if (instance == 0)
    {
      instance = new MedLinacDetectorConstruction(SDName);
     
    }
  return instance;
}

MedLinacDetectorConstruction::~MedLinacDetectorConstruction()
{
  delete detectorMessenger;
  delete pHead;
  delete filterDecorator;
  delete MLCDecorator;
  }

G4VPhysicalVolume* MedLinacDetectorConstruction::Construct()
{
  return ConstructGeom();
}

G4VPhysicalVolume* MedLinacDetectorConstruction::ConstructGeom ()
{
  // Cleanup old geometry

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  //    materials

  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;
  G4int ncomponents;
  G4int natoms;
  G4String name;
  G4String symbol;
  G4double fractionmass;                                           
  G4double massOfMole;   
  G4double pressure; 
  G4double temperature; 

  G4NistManager* nist = G4NistManager::Instance();
  a = 14.00674*g/mole;
  G4Element* elN = new G4Element(name="Nitrogen" ,symbol="N", z=7., a);

  a = 16.00*g/mole;
  G4Element* elO = new G4Element(name="Oxygen",symbol="O", z=8., a);

  a = 1.00794*g/mole;
  G4Element* elH = new G4Element(name="Hydrogen",symbol="H", z=1., a);
 
  density = 1.290*mg/cm3;
  G4Material* Air = new G4Material(name="Air",density, ncomponents=2);
  Air->AddElement(elN, fractionmass=70*perCent);
  Air->AddElement(elO, fractionmass=30*perCent);

  density = 1.000*g/cm3;
  G4Material* H2O = new G4Material(name="Water",density, ncomponents=2);
  H2O->AddElement(elH, natoms=2);
  H2O->AddElement(elO, natoms=1);
  H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

  //density = 19.3*g/cm3;
  density = 18.*g/cm3;
  a = 183.85*g/mole;
  G4Material* W =  nist -> FindOrBuildMaterial("G4_W");

  massOfMole = 1.008*g/mole;
  density = 1.e-25*g/cm3;
  temperature = 2.73*kelvin;
  pressure = 3.e-18*pascal; 
  G4Material* Vacuum = new G4Material("interGalactic", z=1.,massOfMole, 
				      density, kStateGas,temperature, pressure);  

  //------------------------------ beam line along z axis------------------

  //---------jaws position--------

  //----1Y------------------------

  G4double thetaY1 = std::atan(-fieldY1/(100.*cm));// in rad
  G4double JawY1Pos_y = -((4.1*cm+28.*cm)*std::sin(thetaY1)+5.62*cm*std::cos(thetaY1));
  G4double JawY1Pos_z =115.*cm-((4.1*cm+28.*cm)*std::cos(thetaY1))+5.62*cm*std::sin(thetaY1);
  //----2Y-------------------------
  G4double thetaY2 = std::atan(fieldY2/(100.*cm)); // in rad
  G4double JawY2Pos_y = (4.1*cm+28.*cm)*std::sin(thetaY2)+5.62*cm*std::cos(thetaY2);
  G4double JawY2Pos_z = 115.*cm-(4.1*cm+28.*cm)*std::cos(thetaY2)+5.62*cm*std::sin(thetaY2);
  //----1X-------------------------
  G4double thetaX1 = std::atan(-fieldX1/(100.*cm));
  G4double JawX1Pos_x = -((37.9*cm+4.1*cm)*std::sin(thetaX1)+9.61*cm*std::cos(thetaX1));
  G4double JawX1Pos_z =115.*cm-(37.9*cm+4.1*cm)*std::cos(thetaX1)+9.61*cm*std::sin(thetaX1);
   //----2X-------------------------
  G4double thetaX2 = std::atan(fieldX2/(100.*cm));
  G4double JawX2Pos_x = (37.9*cm+4.1*cm)*std::sin(thetaX2)+9.61*cm*std::cos(thetaX2);
  G4double JawX2Pos_z = 115.*cm-(37.9*cm+4.1*cm)*std::cos(thetaX2)+9.61*cm*std::sin(thetaX2);
  //---------rotation jaw1Y--------

  G4RotationMatrix* rotatejaw1Y=new G4RotationMatrix();
  rotatejaw1Y->rotateX(thetaY1);

//---------rotation jaw2Y--------

  G4RotationMatrix* rotatejaw2Y=new G4RotationMatrix();
  rotatejaw2Y->rotateX(-thetaY2);

  //---------rotation jaw1X--------

  G4RotationMatrix*  rotatejaw1X=new G4RotationMatrix();
  rotatejaw1X->rotateY(-thetaX1);

  //---------rotation jaw2X--------

  G4RotationMatrix*  rotatejaw2X=new G4RotationMatrix();
  rotatejaw2X->rotateY(thetaX2);

  //---------------colors----------

  G4Colour  white   (1.0, 1.0, 1.0);
   G4Colour  magenta (1.0, 0.0, 1.0); 
   G4Colour  lblue   (0.20, .50, .85);

  //------------------------------------------------------ volumes

  //------------------------------ experimental hall (world volume)
  //------------------------------ beam line along z axis-------SSD=100cm--------------  

//------------------------------ experimental hall (world volume)
  G4double expHall_x = 3.0*m;
  G4double expHall_y = 3.0*m;
  G4double expHall_z = 3.0*m;
  G4Box* experimentalHall_box = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);
  experimentalHall_log = new G4LogicalVolume(experimentalHall_box, Air,"expHall_log",0,0,0);
  experimentalHall_phys = new G4PVPlacement(0,G4ThreeVector(),"expHall",experimentalHall_log,0,false,0);


 
  //------------------------vacuum box------------------------

  G4double vacudim_x = 9.*cm;
  G4double vacudim_y = 9.*cm;
  G4double vacudim_z = 12.755*cm;
  G4Box* vacuumBlock_box = new G4Box("vacuumBlock_box",vacudim_x, vacudim_y,vacudim_z);
  vacuumBlock_log = new G4LogicalVolume(vacuumBlock_box,Vacuum,"vacuumBlock_log",0,0,0);
  G4double vacublockPos_x = 0.0*m;
  G4double vacublockPos_y = 0.0*m;
  G4double vacublockPos_z = 114.755*cm;
  vacuumBlock_phys = new G4PVPlacement(0,G4ThreeVector(vacublockPos_x,vacublockPos_y,vacublockPos_z),"vacuBlock",vacuumBlock_log,experimentalHall_phys,false,0);


  //----------------Jaw Y1 collimator---------------

  G4double JawY1Dim_x = 9.40*cm;
  G4double JawY1Dim_y = 5.62*cm;
  G4double JawY1Dim_z = 3.98*cm;
  G4Box* JawY1 = new G4Box("JawY1",JawY1Dim_x,
                                  JawY1Dim_y,JawY1Dim_z);
  JawY1_log = new G4LogicalVolume(JawY1,
                                             W,"JawY1_log",0,0,0);



  G4double JawY1Pos_x = 0.0*cm;
  //G4double JawY1Pos_y = fieldY1;
  //G4double JawY1Pos_z = 68.1*cm;
  JawY1_phys = new G4PVPlacement(rotatejaw1Y,
             G4ThreeVector(JawY1Pos_x,JawY1Pos_y,JawY1Pos_z),
             "JawY1",JawY1_log,experimentalHall_phys,false,0);

  //----------------Jaw Y2 collimator---------------

  G4double JawY2Dim_x = 9.23*cm;
  G4double JawY2Dim_y = 5.62*cm;
  G4double JawY2Dim_z = 3.68*cm;
  G4Box* JawY2 = new G4Box("JawY2",JawY2Dim_x,
                                  JawY2Dim_y,JawY2Dim_z);
  JawY2_log = new G4LogicalVolume(JawY2,
                                             W,"JawY1_log",0,0,0);
  G4double JawY2Pos_x = 0.0*cm;
  //G4double JawY2Pos_y = fieldY2;
  //G4double JawY2Pos_z = 68.1*cm;
  JawY2_phys = new G4PVPlacement(rotatejaw2Y,
             G4ThreeVector(JawY2Pos_x,JawY2Pos_y,JawY2Pos_z),
             "JawY2",JawY2_log,experimentalHall_phys,false,0);


 //----------------Jaw X1 collimator---------------
  // needs to be changed to a trap?
  G4double JawX1Dim_x = 8.61*cm;
  G4double JawX1Dim_y = 11.18*cm;
  G4double JawX1Dim_z = 4.28*cm;
  G4Box* JawX1 = new G4Box("JawX1",JawX1Dim_x,
                                 JawX1Dim_y,JawX1Dim_z);
  JawX1_log = new G4LogicalVolume(JawX1,
                                             W,"JawX1_log",0,0,0);
  //G4cout <<"_________________DetectorConstruction fieldX1 "<< fieldX1/cm << G4endl;

  //G4double JawX1Pos_x = fieldX1;
  G4double JawX1Pos_y = 0.0*cm;
  //G4double JawX1Pos_z = 54.2*cm;
  JawX1_phys = new G4PVPlacement(rotatejaw1X,
             G4ThreeVector(JawX1Pos_x,JawX1Pos_y,JawX1Pos_z),
             "JawX1",JawX1_log,experimentalHall_phys,false,0);

  //----------------Jaw X2 collimator---------------

  G4double JawX2Dim_x = 8.91*cm;
  G4double JawX2Dim_y = 13.92*cm;
  G4double JawX2Dim_z = 1.88*cm;
  G4Box* JawX2 = new G4Box("JawX2",JawX2Dim_x,
                                  JawX2Dim_y,JawX2Dim_z);
  JawX2_log = new G4LogicalVolume(JawX2,
                                             W,"JawX2_log",0,0,0);
  //G4double JawX2Pos_x = fieldX2;
  G4double JawX2Pos_y = 0.0*cm;
  //G4double JawX2Pos_z = 54.2*cm;
  JawX2_phys = new G4PVPlacement(rotatejaw2X,
             G4ThreeVector(JawX2Pos_x,JawX2Pos_y,JawX2Pos_z),
             "JawX2",JawX2_log,experimentalHall_phys,false,0);

  //----------------Phantom---------
  phantomDim_x = phantomDim;                                                                                                             
  phantomDim_y = phantomDim;                                                                                                             
  phantomDim_z = phantomDim; 
  G4ThreeVector phantomSize = G4ThreeVector(phantomDim, phantomDim, phantomDim);

  G4double PhantomPos_x = 0.0*m;
  G4double PhantomPos_y = 0.0*m;
  G4double PhantomPos_z = 0.0*cm;

  numberOfVoxelsAlongX=numberOfVoxels;
  numberOfVoxelsAlongY=numberOfVoxels;
  numberOfVoxelsAlongZ=numberOfVoxels;

  PrintParameters(); 
  //................................
  // Mother Volume of Water Phantom
  //................................

  G4Box* solidPhantom =
    new G4Box("phantom", phantomSize.x() / 2., phantomSize.y() / 2., phantomSize.z() / 2.);
  G4LogicalVolume* logicPhantom = new G4LogicalVolume(solidPhantom, Air, "Phantom", 0, 0, 0);

  G4RotationMatrix* rot = new G4RotationMatrix();
  // rot->rotateY(30.*deg);
  PhantomPos_z = 114.755*cm + 0.20055*cm - phantomDist - phantomDim_z/2.0;
  G4ThreeVector positionPhantom = G4ThreeVector(0, 0, PhantomPos_z);
  // G4VPhysicalVolume * physiPhantom =
  new G4PVPlacement(0,  // no rotation
                    positionPhantom,  // at (x,y,z)
                    "Phantom",  // its name
                    logicPhantom,  // its logical volume
                    experimentalHall_phys,  // its mother  volume
                    false,  // no boolean operations
                    0);  // copy number  
  G4ThreeVector voxelSize;
  voxelSize.setX(phantomSize.x()/numberOfVoxelsAlongX);
  voxelSize.setY(phantomSize.y()/numberOfVoxelsAlongY);
  voxelSize.setZ(phantomSize.z()/numberOfVoxelsAlongZ);

  // Replication of Water Phantom Volume.
  // Y Slice
  G4String yRepName("YSlice");
  G4VSolid* solYRep =
    new G4Box(yRepName, phantomSize.x() / 2., voxelSize.y() / 2., phantomSize.z() / 2.);
  G4LogicalVolume* logYRep = new G4LogicalVolume(solYRep, Air, yRepName);
  // G4PVReplica* yReplica =
  new G4PVReplica(yRepName, logYRep, logicPhantom, kYAxis, numberOfVoxelsAlongY, voxelSize.y());
  // X Slice
  G4String xRepName("XSlice");
  G4VSolid* solXRep =
    new G4Box(xRepName, voxelSize.x() / 2., voxelSize.y() / 2., phantomSize.z() / 2.);
  G4LogicalVolume* logXRep = new G4LogicalVolume(solXRep, Air, xRepName);
  // G4PVReplica* xReplica =
  new G4PVReplica(xRepName, logXRep, logYRep, kXAxis, numberOfVoxelsAlongX, voxelSize.x());

  G4String voxelName("phantomVoxel");
  G4VSolid* solidVoxel = new G4Box(voxelName, voxelSize.x() / 2., voxelSize.y() / 2., voxelSize.z() / 2.);
  G4LogicalVolume* logicalVolumeVoxel = new G4LogicalVolume(solidVoxel, Air, voxelName);
  //
  //
  std::vector<G4Material*> phantomMat(2, Air);
  //
  // Parameterisation for transformation of voxels.
  //  (voxel size is fixed in this example.
  //  e.g. nested parameterisation handles material and transfomation of voxels.)
  MedLinacNestedPhantomParameterisation* paramPhantom =
    new MedLinacNestedPhantomParameterisation(voxelSize / 2., numberOfVoxelsAlongZ, phantomMat);
  // G4VPhysicalVolume * physiPhantomSens =
  new G4PVParameterised("PhantomSens",  // their name
                        logicalVolumeVoxel,  // their logical volume
                        logXRep,  // Mother logical volume
                        kUndefined,  // Are placed along this axis
                        numberOfVoxelsAlongZ,  // Number of cells
                        paramPhantom);  // Parameterisation.
  //   Optimization flag is avaiable for,  
  //    kUndefined, kXAxis, kYAxis, kZAxis.
  //

//--------- Visualization attributes -------------------------------


 
  G4VisAttributes* simpleH2OVisAtt= new G4VisAttributes(lblue);
  simpleH2OVisAtt->SetVisibility(true);
  simpleH2OVisAtt->SetForceSolid(true);
   logicPhantom->SetVisAttributes(simpleH2OVisAtt);

   G4VisAttributes* simpleVacuumWVisAtt= new G4VisAttributes(white);
   simpleVacuumWVisAtt->SetVisibility(true);
   simpleVacuumWVisAtt->SetForceWireframe(true);
   vacuumBlock_log->SetVisAttributes(simpleVacuumWVisAtt);

   G4VisAttributes* simpleTungstenSVisAtt= new G4VisAttributes(magenta);
   simpleTungstenSVisAtt->SetVisibility(true);
   simpleTungstenSVisAtt->SetForceSolid(true);
   JawY1_log->SetVisAttributes(simpleTungstenSVisAtt);
   JawY2_log->SetVisAttributes(simpleTungstenSVisAtt);
   JawX1_log->SetVisAttributes(simpleTungstenSVisAtt);
   JawX2_log->SetVisAttributes(simpleTungstenSVisAtt);

 
   G4VisAttributes* simpleWorldVisAtt= new G4VisAttributes(white);
   simpleWorldVisAtt->SetVisibility(false);
   experimentalHall_log ->SetVisAttributes(simpleWorldVisAtt);

    G4VisAttributes* yRepVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    logYRep->SetVisAttributes(yRepVisAtt);
    G4VisAttributes* xRepVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    logXRep->SetVisAttributes(xRepVisAtt);

    // Skip the visualization for those voxels.
    logicalVolumeVoxel->SetVisAttributes(G4VisAttributes::GetInvisible());

   //if you want not to see phantom, jaws e vacuum ...:

   //logicPhantom->SetVisAttributes(simpleWorldVisAtt);
   //JawY1_log->SetVisAttributes(simpleWorldVisAtt);
   //JawY2_log->SetVisAttributes(simpleWorldVisAtt);
   //JawX1_log->SetVisAttributes(simpleWorldVisAtt);
   //JawX2_log->SetVisAttributes(simpleWorldVisAtt);
   //vacuumBlock_log->SetVisAttributes(simpleWorldVisAtt);

  //   Sets a max Step length in the detector
   //G4double maxStep = 0.2*mm;
   logicPhantom->SetUserLimits(new G4UserLimits(maxStep));

   ConstructVolume();

   //G4cout <<"????????????????????numberOfVoxels "<< numberOfVoxels <<G4endl ;
   //G4cout <<"????????????????????numberOfVoxelsAlongX "<< numberOfVoxelsAlongX <<G4endl ;
   //G4cout <<"????????????????????numberOfVoxelsAlongY "<< numberOfVoxelsAlongY <<G4endl ;
   //G4cout <<"????????????????????numberOfVoxelsAlongZ "<< numberOfVoxelsAlongZ <<G4endl ;


   //G4cout <<"????????????????????maxStep "<< maxStep/mm << " mm " <<G4endl ;




   return experimentalHall_phys;
}

void MedLinacDetectorConstruction::PrintParameters()
{ 
  //G4cout <<"jaws1 x position "<< fieldX1/cm << " cm "<<G4endl ;
  //G4cout <<"jaws2 x position "<< fieldX2/cm << " cm "<<G4endl ; 
  //G4cout <<"jaws1 y position "<< fieldY1/cm << " cm "<<G4endl ;
  //G4cout <<"jaws2 y position "<< fieldY2/cm << " cm "<<G4endl ; 
  G4cout <<"************************phantom dimension "<< phantomDim_x/cm << " cm "<<G4endl ;
  G4cout <<"************************phantom dimension "<< phantomDim/cm << " cm "<<G4endl;
  //G4cout <<"************************numberOfVoxelsAlongX "<< numberOfVoxelsAlongX <<G4endl ;
  //G4cout <<"************************numberOfVoxelsAlongY "<< numberOfVoxelsAlongY <<G4endl ;
  //G4cout <<"************************numberOfVoxelsAlongZ "<< numberOfVoxelsAlongZ <<G4endl ;
  //G4cout <<"************************maxStep "<< maxStep/mm << " mm " <<G4endl;
}



void MedLinacDetectorConstruction::SetJawX1Pos_x (G4double val)
{ 
  fieldX1 = val;
}


void MedLinacDetectorConstruction::SetJawX2Pos_x (G4double val)
{
  fieldX2 = val;
}

void MedLinacDetectorConstruction::SetJawY1Pos_y (G4double val)
{
  fieldY1 = val;
}

void MedLinacDetectorConstruction::SetJawY2Pos_y (G4double val)
{
  fieldY2 = val;
  //G4cout <<"==============================DetectorConstruction JawY2  "<< fieldY2/cm<<"cm"<<G4endl;
}

void MedLinacDetectorConstruction::SetPhantomDim (G4double val)
{
  phantomDim = val;
  G4cout <<"==============================phantomDim  "<< phantomDim/mm<<"mm"<<G4endl;
}
void MedLinacDetectorConstruction::SetPhantomDist (G4double val)
{
  phantomDist = val;
  G4cout <<"==============================phantomDist  "<< phantomDist/mm<<"mm"<<G4endl;
}

void MedLinacDetectorConstruction::SetNumberOfVoxels (G4int val)
{
  numberOfVoxels = val;
  //G4cout <<"==============================numberOfVoxels  "<< numberOfVoxels<<G4endl;
}

void MedLinacDetectorConstruction::SetMode (G4String mode)
{
  if(mode == "electron"){
    electronMode = true;
  }else{
    electronMode= false;
  }
}

void MedLinacDetectorConstruction::SetMaxStep (G4double val)
{
  maxStep = val;
  //G4cout <<"==============================maxStep  "<< maxStep/mm<<"mm"<<G4endl;
}
void MedLinacDetectorConstruction::UpdateGeometry()
{ 
  //G4cout <<"@@@@@@@@@@@@@@@@@@@@@UpdateGeometry "<< G4endl;
  //G4RunManager::GetRunManager()->DefineWorldVolume(ConstructGeom());
  G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

void MedLinacDetectorConstruction::ConstructVolume()
{
  pHead ->ConstructComponent(experimentalHall_phys,vacuumBlock_phys);
  filterDecorator ->ConstructComponent( experimentalHall_phys,vacuumBlock_phys);
  MLCDecorator ->ConstructComponent( experimentalHall_phys,vacuumBlock_phys);

}

void  MedLinacDetectorConstruction::ConstructSDandField()
{
  if(!phantomSD)
  {
    phantomSD = new MedLinacPhantomSD (sensitiveDetectorName);
    G4SDManager::GetSDMpointer()->AddNewDetector(phantomSD);
  }
  // Attach sensitive detectors by finding the logical volumes by name
  G4LogicalVolume* lv = 0;
  //lv = G4LogicalVolumeStore::GetInstance()->GetVolume("ROPhantomYDivisionLog");
  //if(lv) lv->SetSensitiveDetector(dummySD);
  lv = G4LogicalVolumeStore::GetInstance()->GetVolume("phantomVoxel");
  if(lv) lv->SetSensitiveDetector(phantomSD);

}