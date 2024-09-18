#ifndef Run_hh
#define Run_hh

#include "globals.hh"
#include <map>
#include <iomanip>

#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Gamma.hh"
#include "G4AnalysisManager.hh"

#include "3.0_DetectorConstruction.hh"
#include "5_PrimaryGenerator.hh"

class Run : public G4Run
{
  public:
    Run();
    ~Run();

  public:
    void SetPrimary(G4ParticleDefinition * particle, G4double energy);
    void CountProcesses(G4String procName);
    void Merge(const G4Run *) override;
    void EndOfRun();

  private:
    G4ParticleDefinition * link_ParticleDefinition = nullptr;
    std::map <G4String,G4int>  fProcCounter;
    double link_Energy;
};

#endif