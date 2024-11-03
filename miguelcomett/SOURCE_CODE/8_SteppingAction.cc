#include "8_SteppingAction.hh"

extern int arguments;

MySteppingAction::MySteppingAction(MyEventAction * eventAction) { fEventAction = eventAction; }
MySteppingAction::~MySteppingAction() {}

void MySteppingAction::UserSteppingAction(const G4Step * step)
{
    G4LogicalVolume * Volume = step -> GetPreStepPoint() -> GetTouchableHandle() -> GetVolume() -> GetLogicalVolume();
    const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());
    G4LogicalVolume * ScoringVolume = detectorConstruction -> GetScoringVolume();

    if (arguments == 1 || arguments == 2)
    {
        std::vector<G4LogicalVolume*> scoringVolumes = detectorConstruction -> GetAllScoringVolumes();
        if (std::find(scoringVolumes.begin(), scoringVolumes.end(), Volume) == scoringVolumes.end()) {return;}
            EDep = step -> GetTotalEnergyDeposit();
            if (EDep == 0.0) { return; }
            fEventAction -> AddEDep(EDep);
    }

    if (arguments == 3) 
    {
        if(Volume != ScoringVolume) { return; }

        G4StepPoint * endPoint = step -> GetPostStepPoint();
        G4String processName = endPoint -> GetProcessDefinedStep() -> GetProcessName();
        Run * run = static_cast <Run *> (G4RunManager::GetRunManager() -> GetNonConstCurrentRun()); 
        run -> CountProcesses(processName);

        G4RunManager::GetRunManager() -> AbortEvent();  // kill event after first interaction
    }
    
    noSecondaryGamma = false;
    noSecondaryElectrons = true;

    if (arguments == 4 || arguments == 5)
    {
        const std::vector<const G4Track*>* secondaries = step -> GetSecondaryInCurrentStep();

        if (noSecondaryGamma)
            for (const auto & secondary : * secondaries)
            {
                G4Track * nonPrimaryTrack = const_cast<G4Track *>(secondary);
                nonPrimaryTrack -> SetTrackStatus(fStopAndKill);
            }
        
        if (noSecondaryElectrons)
            for (const auto & secondary : * secondaries)
            {
                if (secondary->GetDefinition()->GetParticleName() != "gamma")
                {
                    G4Track* nonPrimaryTrack = const_cast<G4Track*>(secondary);
                    nonPrimaryTrack->SetTrackStatus(fStopAndKill);
                }
            }

        std::vector<G4LogicalVolume*> scoringVolumes = detectorConstruction -> GetAllScoringVolumes();
        if (std::find(scoringVolumes.begin(), scoringVolumes.end(), Volume) == scoringVolumes.end()) {return;}
            EDep = step -> GetTotalEnergyDeposit();
            if (EDep == 0.0) { return; }
            fEventAction -> AddEDep(EDep);
    }
}