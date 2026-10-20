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
// ********************************************************************
// $Id: MedLinacAnalysisManager.cc,v 1.9 2006/06/29 16:04:15 gunter Exp $
//
//
// Code developed by: M. Piergentili
//
//
#ifdef  G4ANALYSIS_USE
#include <stdlib.h>
#include <fstream>
#include "MedLinacAnalysisManager.hh"

#include "G4ios.hh"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "AIDA/IManagedObject.h"
#include "AIDA/IAnalysisFactory.h"
#include "AIDA/IHistogramFactory.h"
#include "AIDA/ITupleFactory.h"
#include "AIDA/ITreeFactory.h"
#include "AIDA/ITree.h"
#include "AIDA/ITuple.h"

MedLinacAnalysisManager* MedLinacAnalysisManager::instance = 0;

MedLinacAnalysisManager::MedLinacAnalysisManager(): 
  aFact(0), theTree(0), histFact(0),hBase(0),h1(0),h2(0),h3(0), 
  h4(0),h5(0),h6(0),h7(0),h8(0),h9(0), booked(false), finished(false)
  
{
  //build up  the  factories
  aFact = AIDA_createAnalysisFactory();

  AIDA::ITreeFactory *treeFact = aFact->createTreeFactory(); 
 
  //parameters for the TreeFactory
 
  std::string fileName = "MedLinacDiane.xml";
  theTree = treeFact->create(fileName,"xml",false, true,"uncompress");

  delete treeFact;
 
  histFact = aFact->createHistogramFactory( *theTree );

}
MedLinacAnalysisManager::~MedLinacAnalysisManager() 
{ 
  delete histFact;
  histFact = 0;

  delete theTree;
  histFact = 0;

  delete aFact;
  aFact = 0;
}

MedLinacAnalysisManager* MedLinacAnalysisManager::getInstance()
{
  if (instance == 0) instance = new MedLinacAnalysisManager;
  return instance;
}

void MedLinacAnalysisManager::book() 
{
  // idempotent: histograms must persist (and accumulate) across multiple /run/beamOn calls
  if (booked) return;
   //creating an other 1D histogram ...
  h1 = histFact->createHistogram1D("14010","PDD", //histoID,histo name
				    600,-300.0,300.0); //bins' number, zmin, zmax
  //creating an other 1D histogram ...
  h2 = histFact->createHistogram1D("14020","X Flatness build-up5", //histoID,histo name
				    120,-300.0,300.0); //bins' number, zmin, zmax

  //creating an other 1D histogram ...
  h3 = histFact->createHistogram1D("14030","X Flatness 50mm 5", //histoID,histo name
				    120,-300.0,300.0); //bins' number, zmin, zmax

 //creating an other 1D histogram ...
  h4 = histFact->createHistogram1D("14040","X Flatness 100mm 5", //histoID,histo name
                                    120,-300.0,300.0); //bins' number, zmin, zmax
 //creating an other 1D histogram ...
  h5 = histFact->createHistogram1D("14050","X Flatness 200mm 5", //histoID,histo name
                                    120,-300.0,300.0); //bins' number, zmin, zmax

  h6 = histFact->createHistogram1D("14060","Y Flatness build-up5", //histoID,histo name
				    120,-300.0,300.0); //bins' number, zmin, zmax

  //creating an other 1D histogram ...
  h7 = histFact->createHistogram1D("14070","Y Flatness 50mm 5", //histoID,histo name
				    120,-300.0,300.0); //bins' number, zmin, zmax

 //creating an other 1D histogram ...
  h8 = histFact->createHistogram1D("14080","Y Flatness 100mm 5", //histoID,histo name
                                    120,-300.0,300.0); //bins' number, zmin, zmax
 //creating an other 1D histogram ...
  h9 = histFact->createHistogram1D("14090","Y Flatness 200mm 5", //histoID,histo name
                                    120,-300.0,300.0); //bins' number, zmin, zmax

  booked = true;
}

void MedLinacAnalysisManager::FillHistogram1WithEnergy(G4double z, 
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO1-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (0,0,z)  
  h1->fill(z,energyDeposit);
}

void MedLinacAnalysisManager::FillHistogram2WithEnergy(G4double x, 
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO2-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (x,0,135mm)  
  h2->fill(x,energyDeposit);
} 
void MedLinacAnalysisManager::FillHistogram3WithEnergy(G4double x, 
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO3-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (x,0,135mm)  
  h3->fill(x,energyDeposit);
} 

void MedLinacAnalysisManager::FillHistogram4WithEnergy(G4double x,
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO4-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (x,0,80mm)
  h4->fill(x,energyDeposit);
}
  
void MedLinacAnalysisManager::FillHistogram5WithEnergy(G4double x,
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO5-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (x,0,30mm)
  h5->fill(x,energyDeposit);
}


void MedLinacAnalysisManager::FillHistogram6WithEnergy(G4double y, 
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO6-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (0,y,z)  
  h6->fill(y,energyDeposit);
}

void MedLinacAnalysisManager::FillHistogram7WithEnergy(G4double y, 
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO7-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (0,y,135mm)  
  h7->fill(y,energyDeposit);
}

void MedLinacAnalysisManager::FillHistogram8WithEnergy(G4double y, 
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO8-------------"<<G4endl;
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (0,y,80mm)
  h8->fill(y,energyDeposit);
}

void MedLinacAnalysisManager::FillHistogram9WithEnergy(G4double y, 
                                                    G4float energyDeposit)
{
  //G4cout << " fill HISTO9-------------"<<G4endl; 
  //1DHistrogram: energy deposit in a voxel which center is fixed in position (0,y,30mm)
  h9->fill(y,energyDeposit);
}


void MedLinacAnalysisManager::finish() 
{  
  // idempotent: must only close the tree once, after ALL /run/beamOn calls have completed
  if (finished) return;
  finished = true;

  // write all histograms to file ...
  theTree->commit();

  // close (will again commit) ...
  theTree->close();
}


#endif



