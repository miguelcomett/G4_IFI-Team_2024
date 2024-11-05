#include "8_SteppingAction.hh"

MySteppingAction::MySteppingAction(MyEventAction * eventAction) { fEventAction = eventAction; }
MySteppingAction::~MySteppingAction() {}

void MySteppingAction::UserSteppingAction(const G4Step * step)
{
    Volume = step -> GetPreStepPoint() -> GetTouchableHandle() -> GetVolume() -> GetLogicalVolume();
    const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());
    scoringVolume = detectorConstruction -> GetScoringVolume();

    if (arguments == 1 || arguments == 2)
    {   
        std::vector<G4LogicalVolume*> scoringVolumes = detectorConstruction -> GetAllScoringVolumes();
        if (std::find(scoringVolumes.begin(), scoringVolumes.end(), Volume) == scoringVolumes.end()) {return;}
        {
            EDep = step -> GetTotalEnergyDeposit();
            if (EDep > 0.0) { fEventAction -> AddEDep(EDep); }
            fEventAction -> AddEDep(EDep);
            // G4cout << "Energy deposition (keV): " << EDep << G4endl; 
        }
    }

    if (arguments == 3) 
    {
        if(Volume != scoringVolume) { return; }
        // G4cout << "particle touches target: " << scoringVolume << G4endl;

        endPoint = step -> GetPostStepPoint();
        processName = endPoint -> GetProcessDefinedStep() -> GetProcessName();
        Run * run = static_cast <Run *> (G4RunManager::GetRunManager() -> GetNonConstCurrentRun()); 
        run -> CountProcesses(processName);

        G4RunManager::GetRunManager() -> AbortEvent();  // kill event after first interaction
    }

    if (arguments == 4 || arguments == 5)
    {   
        std::vector<G4LogicalVolume*> scoringVolumes = detectorConstruction -> GetAllScoringVolumes();
        if (std::find(scoringVolumes.begin(), scoringVolumes.end(), Volume) == scoringVolumes.end()) {return;}
        {       
            EDep = step -> GetTotalEnergyDeposit();
            if (EDep > 0.0) { fEventAction -> AddEDep(EDep); }
            // G4cout << "Energy deposition (eV): " << G4BestUnit(EDep, "Energy") << G4endl; 
        }
    }
}