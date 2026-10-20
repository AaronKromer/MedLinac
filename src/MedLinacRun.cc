#include "MedLinacRun.hh"
#include "G4RunManager.hh"
#include "G4VVisManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"

MedLinacRun::MedLinacRun()
{
}

MedLinacRun::~MedLinacRun() {}

void MedLinacRun::RecordEvent(const G4Event* evt)
{
  // No-op: event-level hits are appended to the worker MedLinacRun in
  // MedLinacEventAction::EndOfEventAction(). Visualization is handled there.
}

void MedLinacRun::AddHits(const std::vector<MedLinacHit>& hits)
{
  runHitsCollection.insert(runHitsCollection.end(), hits.begin(), hits.end());
}

void MedLinacRun::Merge(const G4Run* aRun)
{
  const MedLinacRun* localRun = static_cast<const MedLinacRun*>(aRun);
  runHitsCollection.insert(runHitsCollection.end(), localRun->runHitsCollection.begin(), localRun->runHitsCollection.end());
  G4Run::Merge(aRun);
}
