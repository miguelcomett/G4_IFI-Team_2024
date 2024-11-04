#include "7_EventAction.hh"

extern int arguments;

MyEventAction::MyEventAction(MyRunAction *) {fEDep = 0.0;}
MyEventAction::~MyEventAction(){}

void MyEventAction::AddEDep(G4double EDep) { fEDep = fEDep + EDep; }

void MyEventAction::BeginOfEventAction(const G4Event * ) {fEDep = 0.0;}

void MyEventAction::EndOfEventAction(const G4Event * ) 
{ 
    fEDep = fEDep / keV;

    if (arguments == 1)
    {
        G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
        // G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 
    }
    G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();

    if (fEDep == 0.0){return;}
    G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    
    if (arguments == 1 || arguments == 2)
    {
        analysisManager -> FillNtupleDColumn(1, 0, fEDep);
        analysisManager -> AddNtupleRow(1);
    }

    if (arguments == 4)
    {
        analysisManager -> FillNtupleDColumn(1, 0, fEDep);
        analysisManager -> AddNtupleRow(1);
    }
}