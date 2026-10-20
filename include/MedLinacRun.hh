#ifndef MedLinacRun_h
#define MedLinacRun_h

#include "G4Run.hh"
#include <vector>
#include "MedLinacEventAction.hh" // for MedLinacHit

class MedLinacRun : public G4Run
{
 public:
  MedLinacRun();
  virtual ~MedLinacRun();
  virtual void RecordEvent(const G4Event*);
  virtual void Merge(const G4Run*);
  // Append event hits into this run (called from EventAction on workers)
  void AddHits(const std::vector<MedLinacHit>& hits);
  const std::vector<MedLinacHit>& GetHits() const { return runHitsCollection; }

 private:
  
  std::vector<MedLinacHit> runHitsCollection;
};

#endif