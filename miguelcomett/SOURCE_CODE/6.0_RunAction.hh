#ifndef RunAction_hh
#define RunAction_hh

#include "Randomize.hh"
#include <iomanip>
#include <ctime> 
#include <chrono>
#include <iostream>
#include <vector> 
#include <atomic>

#include "G4UIManager.hh"
#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include <G4AccumulableManager.hh>

#include "3.0_DetectorConstruction.hh"
#include "5_PrimaryGenerator.hh"
#include "6.1_Run.hh"

extern int arguments;

class MyRunAction : public G4UserRunAction
{
    public:

        MyRunAction();
        ~MyRunAction(); 

        void BeginOfRunAction(const G4Run * thisRun) override;
        void EndOfRunAction  (const G4Run * thisRun) override;

        G4Run * GenerateRun() override;

        void AddEdep (G4double edep);

        void IncrementEventCount();
        int GetEventCount() const;

    private:
        
        std::atomic<int> eventsProcessed{0};

        Run * customRun = nullptr;

        G4String particleName, directory, fileName;
        G4int numberOfEvents, runID, index;
        G4double sampleMass, primaryEnergy, totalMass, durationInSeconds, TotalEnergyDeposit, radiationDose;
        
        G4Accumulable <G4double> fEdep = 0.0;

        std::chrono::system_clock::time_point simulationStartTime, simulationEndTime;

        const G4double milligray = 1.0e-3*gray;
        const G4double microgray = 1.0e-6*gray;
        const G4double nanogray  = 1.0e-9*gray;
        const G4double picogray  = 1.0e-12*gray;
};

#endif