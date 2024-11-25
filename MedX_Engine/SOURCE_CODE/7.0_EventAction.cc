#include "7.0_EventAction.hh"

EventAction::EventAction(RunAction * runAction) : fRunAction(runAction) {fEDep = 0.0;}
EventAction::~EventAction(){}

void EventAction::BeginOfEventAction(const G4Event * event) {fEDep = 0.0;}
void EventAction::EndOfEventAction(const G4Event * event) 
{ 
    totalEvents = G4RunManager::GetRunManager() -> GetNumberOfEventsToBeProcessed();
    eventID = event -> GetEventID();

    if (eventID == std::ceil(totalEvents*.25)) { std::cout << "\033[32mProgress: 25% \033[32m" << std::endl; }
    if (eventID == std::ceil(totalEvents*.50)) { std::cout << "\033[32mProgress: 50% \033[32m" << std::endl; }
    if (eventID == std::ceil(totalEvents*.75)) { std::cout << "\033[32mProgress: 75% \033[32m" << std::endl; }

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