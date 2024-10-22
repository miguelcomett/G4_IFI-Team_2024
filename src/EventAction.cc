#include "EventAction.hh"
#include "G4AnalysisManager.hh"

namespace G4_PCM {
    // Initialize static member
    int EventAction::fNtupleRegistrationCount = 0;

    EventAction::EventAction(RunAction*) {
        fEnergy = 0. / keV;
        fWeight = 0;
    }

    void EventAction::BeginOfEventAction(const G4Event* anEvent) {
        fEnergy = 0. / keV;
    }

    void EventAction::AddEnergy(G4double e) {
        fEnergy += e;
    }

    void EventAction::SetPosition(G4ThreeVector p) {
        fPosition = p;
    }

    void EventAction::AddWeight(G4double w)
    {
        fWeight = w;
    }

    void EventAction::EndOfEventAction(const G4Event* anEvent) {
        if (fEnergy > 0.0) {
            G4int energyColumnId = 0;
            G4int posXColumnId = 1;
            G4int posYColumnId = 2;
            G4int posZColumnId = 3;

            G4AnalysisManager* man = G4AnalysisManager::Instance();
            man->FillNtupleDColumn(0, energyColumnId, fEnergy / keV);
            man->FillNtupleDColumn(0, posXColumnId, fPosition.getX());
            man->FillNtupleDColumn(0, posYColumnId, fPosition.getY());
            man->FillNtupleDColumn(0, posZColumnId, fPosition.getZ());
            man->AddNtupleRow(0);
            man->FillH1(0, fEnergy, fWeight);

            //man->FillH1(0, fEnergy, fWeight);

            // Increment the registration count
            ++fNtupleRegistrationCount;
        }
    }

    void EventAction::Print() {
        G4cout
            << "Energy: "
            << G4BestUnit(fEnergy, "Energy")
			<< "Position: "
			<< G4BestUnit(fPosition.getX(), "Length")
			<< G4BestUnit(fPosition.getY(), "Length")
			<< G4BestUnit(fPosition.getZ(), "Length")
            << G4endl;
    }

    int EventAction::GetNtupleRegistrationCount() {
        return fNtupleRegistrationCount;
    }

    void EventAction::ResetNtupleRegistrationCount() {
        fNtupleRegistrationCount = 0;
    }
}
