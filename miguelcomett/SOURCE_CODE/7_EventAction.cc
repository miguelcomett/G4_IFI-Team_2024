#include "7_EventAction.hh"

extern int arguments;

MyEventAction::MyEventAction(MyRunAction *) 
{ 
    fEDep = 0.0; 
    fEnergy = 0.0;
}
MyEventAction::~MyEventAction(){}

void MyEventAction::BeginOfEventAction(const G4Event * anEvent) 
{
    fEDep = 0.0; 
    fEnergy = 0.0;
}

void MyEventAction::AddEDep(G4double EDep) { fEDep = fEDep + EDep; }
void MyEventAction::AddEnergy(G4double e) { fEnergy = fEnergy + e; }

void MyEventAction::EndOfEventAction(const G4Event * anEvent) 
{ 
    fEDep = fEDep / keV;

    if (arguments == 1)
    {
        G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
        G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 
    }
    
    if (fEDep > 0.1 * keV && (arguments == 1 || arguments == 2))
    {
        G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
        analysisManager -> FillNtupleDColumn(0, 5, fEDep);
        analysisManager -> AddNtupleRow(0);
    }
}