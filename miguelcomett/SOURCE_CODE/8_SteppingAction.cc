#include "8_SteppingAction.hh"

extern int arguments;

MySteppingAction::MySteppingAction(MyEventAction * eventAction) { fEventAction = eventAction; }
MySteppingAction::~MySteppingAction() {}

void MySteppingAction::UserSteppingAction(const G4Step * step)
{

    if (arguments == 2 || arguments == 4) 
    {
        G4StepPoint * endPoint = step -> GetPostStepPoint();
        G4String procName = endPoint -> GetProcessDefinedStep() -> GetProcessName();
        Run * run = static_cast <Run *> (G4RunManager::GetRunManager() -> GetNonConstCurrentRun()); 
        run -> CountProcesses(procName);

        G4RunManager::GetRunManager() -> AbortEvent();  // kill event after first interaction
    }
    
    if (arguments == 5)
    {
        const std::vector <const G4Track*> * secondaries = step -> GetSecondaryInCurrentStep();

        for (const auto & secondary : * secondaries)
        {        
            if (secondary->GetDefinition()->GetParticleName() == "gamma") // This ensures we only kill secondaries
            { 
                G4Track * nonPrimaryTrack = const_cast <G4Track*> (secondary);
                nonPrimaryTrack -> SetTrackStatus(fStopAndKill);
            }
        }

    }

    if (arguments == 1 || arguments == 3)
    {
        G4LogicalVolume * Volume = step -> GetPreStepPoint() -> GetTouchableHandle() -> GetVolume() -> GetLogicalVolume();
        const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());
        
        G4LogicalVolume * fScoringVolume = detectorConstruction -> GetScoringVolume();
        if(Volume != fScoringVolume) { return; }

        G4double EDep = step -> GetTotalEnergyDeposit();
        fEventAction -> AddEdep(EDep);
    }

}