#ifndef RunAction_hh
#define RunAction_hh

#include "Randomize.hh"
#include <iomanip>
#include <ctime> 
#include <chrono>
#include <iostream>

#include "G4UIManager.hh"
#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4Run.hh"
#include <G4AccumulableManager.hh>

#include "3.0_DetectorConstruction.hh"
#include "5_PrimaryGenerator.hh"
#include "6.1_Run.hh"

class MyRunAction : public G4UserRunAction
{
    public:

        MyRunAction();
        ~MyRunAction(); 

        void BeginOfRunAction(const G4Run * run) override;
        void EndOfRunAction  (const G4Run * run) override;

        // void SetPrimary(G4ParticleDefinition * particle, G4double energy);

        G4Run * GenerateRun() override;
        
    private:

        Run * fRun = nullptr;
        G4String dataTitle;
        G4double sampleMass;

        G4String particleName;
        G4double primaryEnergy;
        G4int numberOfEvents;
};

#endif