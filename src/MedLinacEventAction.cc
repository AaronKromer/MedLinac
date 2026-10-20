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
// $Id: MedLinacEventAction.cc,v 1.5 2006/06/29 16:04:23 gunter Exp $
//
//
// Code developed by: M. Piergentili
//
#include <G4SystemOfUnits.hh>

#include "MedLinacEventAction.hh"
#include "MedLinacPhantomHit.hh"
#include "MedLinacPhantomSD.hh"
#include "MedLinacDetectorConstruction.hh"
#include "MedLinacRunAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "MedLinacRun.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"

#ifdef G4ANALYSIS_USE
#include"MedLinacAnalysisManager.hh"
#endif


 // Retrieve information about the energy deposit in the phantom ...

MedLinacEventAction::MedLinacEventAction() :
  drawFlag("all" )
{
 }

 
MedLinacEventAction::~MedLinacEventAction()
{
 }
 
void MedLinacEventAction::BeginOfEventAction(const G4Event*)
{
  // clear per-event container (member) so each event starts empty
  ClearHits();

}
void MedLinacEventAction::ClearHits()
{
  hitsCollection.clear();
}

void MedLinacEventAction::AddHit(const MedLinacHit& h)
{
  hitsCollection.push_back(h);
}

const std::vector<MedLinacHit>& MedLinacEventAction::GetHits() const
{
  return hitsCollection;
}

 
void MedLinacEventAction::EndOfEventAction(const G4Event* evt)
{  
 // extract the trajectories and draw them ...
  if (G4VVisManager::GetConcreteInstance())
    {
      G4TrajectoryContainer * trajectoryContainer = evt->GetTrajectoryContainer();
      G4int n_trajectories = 0;
      if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

      for (G4int i=0; i<n_trajectories; i++) 
        {
          G4Trajectory* trj = (G4Trajectory*)
                                ((*(evt->GetTrajectoryContainer()))[i]);
	  if(drawFlag == "all") trj->DrawTrajectory();
	  else if((drawFlag == "charged")&&(trj->GetCharge() != 0.))
	    trj->DrawTrajectory();
	  else if ((drawFlag == "neutral")&&(trj->GetCharge() == 0.))
	    trj->DrawTrajectory();	     	     
	}
    }
  // get current run and cast to MedLinacRun, then append event hits into the worker run
  G4Run* run = const_cast<G4Run*>(G4RunManager::GetRunManager()->GetCurrentRun());
  MedLinacRun* mrun = static_cast<MedLinacRun*>(run);
  if (mrun) {
    mrun->AddHits(hitsCollection);
  }
  hitsCollection.clear();

 }


G4bool MedLinacEventAction::AnalyzeHit(const MedLinacHit& hit, G4bool fillHistograms)
{ 
  float x = hit.x;
  float y = hit.y;
  float z = hit.z;
  G4double energyDep = hit.energyDep;
  G4bool hitUsed = false;
  MedLinacAnalysisManager* analysis = nullptr;
  if (fillHistograms) {
    analysis = MedLinacAnalysisManager::getInstance();
  }
//**** PDD in isocenter (Y and X Thickness = 5. mm) ***********

  const G4bool applyChamberCuts = true;

  if (!applyChamberCuts || (x<=2.5*mm && x>= -2.5*mm))
  {
    if (!applyChamberCuts || (y<=2.5*mm && y>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram1WithEnergy(z,energyDep/MeV);
      }
      hitUsed = true;
    }
  }

//***** flatness  along x ***** Depth=build-up (15 mm)*************************
  if (z<=287.5*mm && z>=282.5*mm)
  {
    if (!applyChamberCuts || (y<=2.5*mm && y>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram2WithEnergy(x,energyDep/MeV);
      }
      hitUsed = true;
    }
  }

//***** flatness  along x ***** Depth=50mm 5*********************************
  if (z<=252.5*mm && z>= 247.5*mm)
  {
    if (!applyChamberCuts || (y<=2.5*mm && y>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram3WithEnergy(x,energyDep/MeV);
      }
      hitUsed = true;
    }
  }

//***** flatness  along x ***** Depth=100 mm********************************
  if (z<=202.5*mm && z>= 197.5*mm)
  {
    if (!applyChamberCuts || (y<=2.5*mm && y>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram4WithEnergy(x,energyDep/MeV);
      }
      hitUsed = true;
    }
  }

//***** flatness  along x ***** Depth=200 mm********************************
  if (z<=52.5*mm && z>= 47.5*mm)
  {
    if (!applyChamberCuts || (y<=2.5*mm && y>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram5WithEnergy(x,energyDep/MeV);
      }
      hitUsed = true;
    }
  }

//***** flatness along y ***** Depth=build-up (15 mm) ***********************
  if (z<=287.5*mm && z>=282.5*mm)
  {
    if (!applyChamberCuts || (x<=2.5*mm && x>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram6WithEnergy(y,energyDep/MeV);
      }
        hitUsed = true;
    }
  }

//***** flatness along y ***** Depth=50 mm ***********************************
  if (z<=252.5*mm && z>= 247.5*mm)
  {
    if (!applyChamberCuts || (x<=2.5*mm && x>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram7WithEnergy(y,energyDep/MeV);
      }
      hitUsed = true;
    }
  }

//***** flatness along y ***** Depth=100 mm *********************************
  if (z<=202.5*mm && z>= 197.5*mm)
  {
    if (!applyChamberCuts || (x<=2.5*mm && x>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram8WithEnergy(y,energyDep/MeV);
      }
      hitUsed = true;
    }
  }

//***** flatness along y ***** Depth=200 mm *********************************
  if (z<=52.5*mm && z>= 47.5*mm)
  {
    if (!applyChamberCuts || (x<=2.5*mm && x>= -2.5*mm))
    {
      if (fillHistograms) {
        analysis->FillHistogram9WithEnergy(y,energyDep/MeV);
      }
      hitUsed = true;
    }
  }
  return hitUsed ;
}

