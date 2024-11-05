#include "7_EventAction.hh"

extern int arguments;

MyEventAction::MyEventAction(MyRunAction * runAction) : fRunAction(runAction) {fEDep = 0.0;}
MyEventAction::~MyEventAction(){}

void MyEventAction::AddEDep(G4double EDep) { fEDep = fEDep + EDep; }

void MyEventAction::BeginOfEventAction(const G4Event * event) {fEDep = 0.0;}
void MyEventAction::EndOfEventAction(const G4Event * event) 
{ 
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    
    // fEDep = fEDep / keV;

    if (arguments == 1 || arguments == 2)
    {
        if (fEDep > 0.0) {analysisManager -> FillNtupleDColumn(1, 0, fEDep);}
        analysisManager -> AddNtupleRow(1);
    }
    
    if (arguments == 4)
    {
        if (fEDep == 0.0) {analysisManager -> FillNtupleDColumn(1, 0, fEDep);}
        analysisManager -> AddNtupleRow(1);
    }

    fRunAction -> AddEdep(fEDep);
}

// G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
// G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 