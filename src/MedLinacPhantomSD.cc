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
//// $Id: MedLinacPhantomSD.cc,v 1.7 2006/06/29 16:04:37 gunter Exp $
//
//
// Code developed by: M. Piergentili

//
//
#include <G4SystemOfUnits.hh>
#include "MedLinacPhantomSD.hh"
#include "MedLinacPhantomMessenger.hh"
#include "MedLinacPhantomHit.hh"
#include "MedLinacEventAction.hh"
#include "MedLinacAnalysisManager.hh"
#include "G4EventManager.hh"
#include "MedLinacDetectorConstruction.hh"
#include "G4Track.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"

//....

MedLinacPhantomSD::MedLinacPhantomSD(G4String name):G4VSensitiveDetector(name),
  phantomDimension(0.), numberOfPhantomVoxels(0),
  fDebugTotalHits(0), fDebugCentralZHits(0), fDebugCentralXHits(0), fDebugCentralYHits(0),
  fDebugPrintedSamples(0)
{
 phantomMessenger = new MedLinacPhantomMessenger(this);
}

MedLinacPhantomSD::~MedLinacPhantomSD()
{
  G4cout << "CENTRAL_HIT_DEBUG [thread summary] totalHits=" << fDebugTotalHits
         << " centralZHits=" << fDebugCentralZHits
         << " centralXHits=" << fDebugCentralXHits
         << " centralYHits=" << fDebugCentralYHits
         << G4endl;
  delete phantomMessenger;
}

void MedLinacPhantomSD::Initialize(G4HCofThisEvent*)
{
}

void MedLinacPhantomSD::SetPhantomDimension (G4double val)
{
  phantomDimension = val;
  //G4cout <<"2==============================phantomDim  "<< phantomDimension/mm<<"mm"<<G4endl;
}

void MedLinacPhantomSD::SetNumberOfPhantomVoxels (G4int val)
{
  numberOfPhantomVoxels = val;
  //G4cout <<"2==============================numberOfVoxels  "<< numberOfPhantomVoxels<<G4endl;
}

G4bool MedLinacPhantomSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{
  const G4VTouchable* touch = aStep->GetPreStepPoint()->GetTouchable();

  // Replica/parameterisation layout in this geometry:
  // depth 0 : parameterised Z (voxel)
  // depth 1 : X replica (mother of voxel)
  // depth 2 : Y replica (grandmother)
  G4int k = touch->GetCopyNumber(0); // Z index (depth 0)
  G4int i = touch->GetCopyNumber(1); // X index (depth 1)
  G4int j = touch->GetCopyNumber(2); // Y index (depth 2)

  G4double energyDep = aStep->GetTotalEnergyDeposit();
  if(energyDep == 0.)
  return false;
  G4int numberOfVoxelZ = numberOfPhantomVoxels;
  G4float voxelWidthZ = phantomDimension/numberOfVoxelZ;
  G4float x = ((- numberOfVoxelZ+1+2*i)*voxelWidthZ)/2; 
  G4float y = ((- numberOfVoxelZ+1+2*j)*voxelWidthZ)/2;
  G4float z = ((- numberOfVoxelZ+1+2*k)*voxelWidthZ)/2;
  

  struct MedLinacHit hit1= {x,y,z,energyDep};
  G4EventManager* eventManager = G4EventManager::GetEventManager();
  G4UserEventAction* eventAction = (eventManager ? eventManager->GetUserEventAction() : nullptr);
  G4bool collectHit = MedLinacEventAction::AnalyzeHit(hit1, false);
  if (eventAction && collectHit) {
    MedLinacEventAction* mla = static_cast<MedLinacEventAction*>(eventAction);
    mla->AddHit(hit1);
  }
  return true;
}

void MedLinacPhantomSD::EndOfEvent(G4HCofThisEvent*)
{
}

void MedLinacPhantomSD::clear()
{
} 

void MedLinacPhantomSD::DrawAll()
{
}

void MedLinacPhantomSD::PrintAll()
{
}

