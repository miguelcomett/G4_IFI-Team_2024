#include "RunAction.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4Run.hh"
#include "EventAction.hh" // Include EventAction for access to the static method

namespace G4_PCM {
    RunAction::RunAction() {
        auto analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetDefaultFileType("root");
        analysisManager->SetNtupleMerging(true);
        analysisManager->SetVerboseLevel(0);
        analysisManager->CreateNtuple("Photons", "Hits");
        analysisManager->CreateNtupleDColumn("Energy");//   id = 0
        analysisManager->CreateNtupleDColumn("X_axis"); //id = 1
        analysisManager->CreateNtupleDColumn("Y_axis"); //id = 2
        analysisManager->CreateNtupleDColumn("Z_axis"); //id = 3
        analysisManager->FinishNtuple();
    }

    RunAction::~RunAction() {
    }

    void RunAction::BeginOfRunAction(const G4Run* aRun) {
        fTimer.Start();
        auto* analysisManager = G4AnalysisManager::Instance();
        G4int runNumber = aRun->GetRunID();
        G4String fileName = "Sim" + std::to_string(runNumber);
        analysisManager->SetFileName(fileName);
        analysisManager->OpenFile();

        // Reset the ntuple registration count at the beginning of each run
        EventAction::ResetNtupleRegistrationCount();
    }

    void RunAction::EndOfRunAction(const G4Run* aRun) {
        auto* analysisManager = G4AnalysisManager::Instance();
        analysisManager->Write();
        analysisManager->CloseFile();
        fTimer.Stop();
        PrintTime();

        // Print the total number of ntuple registrations
        G4cout
            << "Total number of hits registrations: "
            << EventAction::GetNtupleRegistrationCount()
            << G4endl;
    }

    void RunAction::PrintTime() {
        auto time = fTimer.GetRealElapsed();
        G4cout
            << "Elapsed time: "
            << time
            << " Seconds."
            << G4endl;
    }
}
