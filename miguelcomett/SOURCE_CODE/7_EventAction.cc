#include "7_EventAction.hh"

extern int arguments;

MyEventAction::MyEventAction(MyRunAction *) { fEDep = 0.0; }
MyEventAction::~MyEventAction(){}

void MyEventAction::BeginOfEventAction(const G4Event *) { fEDep = 0.0; }

void MyEventAction::EndOfEventAction(const G4Event *) 
{ 
    if(arguments == 1 || arguments == 3) 
    { 
        G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();

        fEDep = fEDep / keV;

        if (arguments == 1 || arguments == 2)
        {
            G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 
        }

        G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
        analysisManager -> FillNtupleDColumn(2, 0, fEDep);
        analysisManager -> AddNtupleRow(2);
    }
}