#include "RunAction.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4Run.hh"
#include "EventAction.hh"  // Include EventAction for access to the static method
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

namespace G4_PCM {

    RunAction::RunAction() : G4UserRunAction(), timer(nullptr) {
        // Create and configure the analysis manager
        auto* analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetDefaultFileType("root");
        analysisManager->SetNtupleMerging(true);
        analysisManager->SetVerboseLevel(0);

        // Create ntuple
        analysisManager->CreateNtuple("Photons", "Hits");
        analysisManager->CreateNtupleDColumn("Energy");  // id = 0
        analysisManager->CreateNtupleDColumn("X_axis");  // id = 1
        analysisManager->CreateNtupleDColumn("Y_axis");  // id = 2
        analysisManager->CreateNtupleDColumn("Z_axis");  // id = 3
        analysisManager->FinishNtuple();

        // Initialize the timer
        timer = new G4Timer;
    }

    RunAction::~RunAction() {
        // Delete the analysis manager and timer
        delete G4AnalysisManager::Instance();
        delete timer;
    }

    void RunAction::BeginOfRunAction(const G4Run* aRun) {
        // Start timer
        timer->Start();

        // Get the analysis manager instance
        auto* analysisManager = G4AnalysisManager::Instance();

        // Set filename for this run
        G4int runNumber = aRun->GetRunID();
        G4String fileName = "Sim" + std::to_string(runNumber);
        analysisManager->SetFileName(fileName);
        analysisManager->OpenFile();

        // Reset ntuple registration count at the beginning of each run
        EventAction::ResetNtupleRegistrationCount();
    }

    void RunAction::EndOfRunAction(const G4Run* aRun) {
        // Stop timer
        timer->Stop();

        // Save and close the analysis file
        auto* analysisManager = G4AnalysisManager::Instance();
        analysisManager->Write();
        analysisManager->CloseFile();

        // Print the elapsed time
        PrintTime();

        // Print the total number of hits registered
        G4cout
            << "Total number of hits registrations: "
            << EventAction::GetNtupleRegistrationCount()
            << G4endl;
    }

    void RunAction::PrintTime() {
        // Print the real elapsed time for the run
        auto time = timer->GetRealElapsed();
        G4cout
            << "Elapsed time: "
            << G4BestUnit(time, "Time")
            << G4endl;
    }
}
