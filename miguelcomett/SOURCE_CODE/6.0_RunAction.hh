#ifndef RunAction_hh
#define RunAction_hh

#include "Randomize.hh"
#include <iomanip>
#include <ctime> 
#include <chrono>
#include <iostream>
#include <vector> 

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

        void BeginOfRunAction(const G4Run * thisRun) override;
        void EndOfRunAction  (const G4Run * thisRun) override;

        void SetStartTime(const std::chrono::time_point<std::chrono::system_clock> & startTime);

        G4Run * GenerateRun() override;
        
    private:

        Run * customRun = nullptr;

        G4String particleName, directory, fileName;
        G4int numberOfEvents, runID, index;
        G4double sampleMass, primaryEnergy, totalMass, durationInSeconds;

        std::chrono::system_clock::time_point simulationStartTime, simulationEndTime;

        // Gint runIndex = 0;
};

#endif