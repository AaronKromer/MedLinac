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
// $Id: MedLinacEventAction.hh,v 1.5 2006/06/29 16:03:41 gunter Exp $
//
//
// Code developed by: M. Piergentili
//
 
#ifndef MedLinacEventAction_h
#define MedLinacEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

//class G4Event;
//class MedLinacDetectorConstruction; 
//class MedLinacAnalysisManager;

//*********************************************************************
struct MedLinacHit {
  float x;
  float y;
  float z;
  double energyDep;
};


class MedLinacEventAction : public G4UserEventAction
{
  public:
    MedLinacEventAction();
   ~MedLinacEventAction();

  public:
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void AddHit(const MedLinacHit& h);
    void ClearHits();
    static G4bool AnalyzeHit(const MedLinacHit& hit, G4bool fillHistograms = false);
    const std::vector<MedLinacHit>& GetHits() const;
    

private:
  std::vector<MedLinacHit> hitsCollection; 
  G4String drawFlag; //Visualisation flag
  };

//*********************************************************************

#endif

    
