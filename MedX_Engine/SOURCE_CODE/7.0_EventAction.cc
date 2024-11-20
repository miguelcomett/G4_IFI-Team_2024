#include "7.0_EventAction.hh"

EventAction::EventAction(RunAction * runAction) : fRunAction(runAction) {fEDep = 0.0;}
EventAction::~EventAction(){}

void EventAction::AddEDep(G4double EDep) { fEDep = fEDep + EDep; }

void EventAction::BeginOfEventAction(const G4Event * event) {fEDep = 0.0;}
void EventAction::EndOfEventAction(const G4Event * event) 
{ 
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    if (arguments == 1 || arguments == 2)
    {
        if (fEDep > 0.0) 
        {
            analysisManager -> FillNtupleDColumn(2, 0, fEDep);
            analysisManager -> AddNtupleRow(2);
        }
    }
    
    if (arguments == 4)
    {
        if (fEDep > 0.0) 
        {
            EDep_keV = fEDep / keV;
            // G4cout << "Energy deposition (keV): " << EDep_keV << G4endl;

            analysisManager -> FillNtupleDColumn(2, 0, EDep_keV);
            analysisManager -> AddNtupleRow(2);
        }
    }

    fRunAction -> AddEdep(fEDep);
}

// G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
// G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 