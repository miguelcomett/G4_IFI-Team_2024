#include "7_EventAction.hh"

extern int arguments;

MyEventAction::MyEventAction(MyRunAction * runAction) : fRunAction(runAction) {fEDep = 0.0;}
MyEventAction::~MyEventAction(){}

void MyEventAction::AddEDep(G4double EDep) { fEDep = fEDep + EDep; }

void MyEventAction::BeginOfEventAction(const G4Event * event) 
{
    fEDep = 0.0;
}

void MyEventAction::EndOfEventAction(const G4Event * event) 
{ 
    fEDep = fEDep / keV;

    const G4Run * thisRun = G4RunManager::GetRunManager() -> GetCurrentRun();
    eventID = event -> GetEventID();
    numberOfEvents = thisRun->GetNumberOfEventToBeProcessed();

    // const Run * currentRun = static_cast<const Run *>();
    // primaryEnergy = currentRun -> GetPrimaryEnergy();

    if (eventID == 0) 
    {
        G4cout << G4endl; G4cout << G4endl;
        G4cout << "++++++++++++++++++++Este es el primer evento." << G4endl;
        // G4cout << "Energía: " << primaryEnergy << G4endl;
    }

    if (eventID == numberOfEvents - 1) 
    {
        G4cout << "--------------------Este es el último evento: " << eventID <<G4endl;
    }

    EDepBuffer = fEDep;

    if (fEDep == 0.0){return;}
    G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
    // G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    if (arguments == 1 || arguments == 2)
    {
        analysisManager -> FillNtupleDColumn(1, 0, fEDep);
        analysisManager -> AddNtupleRow(1);
    }

    if (arguments == 4)
    {
        // analysisManager -> FillNtupleDColumn(1, 0, fEDep);
        // analysisManager -> AddNtupleRow(1);
    }

    fRunAction -> AddEdep(fEDep);
}