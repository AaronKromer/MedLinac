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
// $Id: MedLinac.cc,v 1.12 2007/07/01 15:19:18 mpiergen Exp $
//
// --------------------------------------------------------------
//      GEANT 4 -  medical_linac
//
// Code developed by: M. Piergentili


#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4UImessenger.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh" 


#include "MedLinacDetectorMessenger.hh"
#include "MedLinacDetectorConstruction.hh"
#include "MedLinacPhysicsList.hh"
#include "MedLinacPhantomSD.hh"
#include "MedLinacActionInitialization.hh"
#include "MedLinacPhantomHit.hh"
#include "MedLinacPrimaryGeneratorAction.hh"
#include "MedLinacEventAction.hh"
#include "MedLinacRunAction.hh"
#include "MedLinacTrackingAction.hh"
#include "G4SDManager.hh"
#include "G4Timer.hh"
#include "G4RunManagerFactory.hh"

#ifdef G4ANALYSIS_USE
#include "MedLinacAnalysisManager.hh"
#endif

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

int main(int argc ,char ** argv)
{

//choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::MixMaxRng);
  G4int seed = time(NULL) + getpid();
  CLHEP::HepRandom :: setTheSeed(seed);
  G4Timer* eventTimer = new G4Timer;
  eventTimer->Start();
 // Construct the default run manager
  //G4RunManager* runManager = new G4RunManager;
  G4int nThreads = 1;
  auto* runManager = G4RunManagerFactory::CreateRunManager();
  if (argc > 2)
  {
    nThreads = G4UIcommand::ConvertToInt(argv[2]);
    if (nThreads < 0) {
      G4cerr << "Number of threads should be non-negative. Using single thread."
             << G4endl;
      nThreads = 1;
    }
    if (nThreads == 0) {
      nThreads = G4Threading::G4GetNumberOfCores();
      G4cout << "Auto-threading: using " << nThreads 
           << " threads (all available cores)." << G4endl;
    }
  }
  runManager->SetNumberOfThreads(nThreads);
  G4String sensitiveDetectorName = "Phantom";
  MedLinacDetectorConstruction* pDetectorConstruction = 
MedLinacDetectorConstruction::GetInstance(sensitiveDetectorName);


  // set mandatory initialization classes
  runManager->SetUserInitialization(pDetectorConstruction);
  runManager->SetUserInitialization(new MedLinacPhysicsList);


#ifdef G4VIS_USE
  // Visualization manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif


  // output environment variables:
#ifdef G4ANALYSIS_USE
  G4cout << G4endl << G4endl << G4endl 
	 << " User Environment " << G4endl
	 << " Using AIDA 3.0 analysis " << G4endl;
# else
  G4cout << G4endl << G4endl << G4endl 
	 << " User Environment " << G4endl
	 << " G4ANALYSIS_USE environment variable not set, NO ANALYSIS " 
	 << G4endl;
#endif
  
  G4UIExecutive* ui = nullptr;
  if (argc == 1) // Define UI session for interactive mode.
  {
    ui = new G4UIExecutive(argc, argv, "Qt");
  }




  // set mandatory user action class
  
  //runManager->SetUserAction(new MedLinacPrimaryGeneratorAction);
  //MedLinacEventAction *pMedLinacEventAction = new MedLinacEventAction();
  //runManager->SetUserAction(pMedLinacEventAction);

  //MedLinacRunAction *pRunAction = new MedLinacRunAction();

  //runManager->SetUserAction(pRunAction);  
  //runManager->SetUserAction(new MedLinacTrackingAction);

  MedLinacActionInitialization* action= new MedLinacActionInitialization();
  runManager->SetUserInitialization(action);


  // Initialize  G4 kernel
  //runManager->Initialize();

  // get the pointer to the UI manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  
 if (ui)   // Define UI session for interactive mode.
    { 
      G4cout<<" UI session starts ..."<< G4endl; 
      UImanager -> ApplyCommand("/control/execute vis.mac");
      ui->SessionStart();
      delete ui;    
    }

  else           // Batch mode
    { 
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      UImanager ->ApplyCommand(command+fileName);
    }  


  // Commit/close the analysis output once, after ALL /run/beamOn calls in
  // this job (whether run interactively or via the batch macro) have finished.
#ifdef G4ANALYSIS_USE
  MedLinacAnalysisManager::getInstance()->finish();
#endif

  //job termination

#ifdef G4VIS_USE
 delete visManager;
#endif

 

 eventTimer->Stop();
 G4cout << "Total execution time: " << eventTimer->GetRealElapsed() << " s" << G4endl;
 G4cout << "Number of threads: " << nThreads << G4endl;


 delete eventTimer;
 delete runManager;
 return 0;
}



