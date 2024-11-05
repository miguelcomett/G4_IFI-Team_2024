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

        void SetPrimary(G4ParticleDefinition * particle, G4double energy);
        G4ParticleDefinition * link_ParticleDefinition = nullptr;

        void CountProcesses(G4String processName);
        void Merge(const G4Run *) override;
        void EndOfRun();

        G4String GetPrimaryParticleName() const;
        G4double GetPrimaryEnergy() const;


    private:

        std::map <G4String, G4int> processCounter;

        G4Material * material;
        
        G4String particleName, processName;
        G4int digits, defaultDecimals, totalCount, survive, count, localCount;
        G4double link_Energy, thickness, density, ratio, crossSection, Coefficient;
};

#endif