#include "3.1_DetectorAction.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name){}
MySensitiveDetector::~MySensitiveDetector(){}

G4bool MySensitiveDetector::ProcessHits(G4Step * aStep, G4TouchableHistory * ROhist)
{
    G4Track * particleTrack = aStep -> GetTrack();
    particleTrack -> SetTrackStatus(fStopAndKill);
    G4StepPoint * preStepPoint = aStep -> GetPreStepPoint();
    G4StepPoint * postStepPoint = aStep -> GetPostStepPoint();
    
    G4ThreeVector posPhoton = preStepPoint -> GetPosition();
    G4ThreeVector momPhoton = preStepPoint -> GetMomentum();
    
    Energy = preStepPoint -> GetKineticEnergy() / keV;
    Wavelength = (1.239841939 * eV / momPhoton.mag()) *  1E+03;
    
    const G4VTouchable * touchable = aStep -> GetPreStepPoint() -> GetTouchable();
    copyNo = touchable -> GetCopyNumber();
    G4VPhysicalVolume * detectorVolume = touchable -> GetVolume();
    G4ThreeVector posDetector = detectorVolume -> GetTranslation();

    Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    
    if (arguments == 1 || arguments == 2)
    {
        analysisManager -> FillNtupleIColumn(0, 0, Event);
        analysisManager -> FillNtupleDColumn(0, 1, posPhoton[0]);
        analysisManager -> FillNtupleDColumn(0, 2, posPhoton[1]);
        analysisManager -> FillNtupleDColumn(0, 3, posPhoton[2]);
        if (Wavelength > 0.0) 
            analysisManager -> FillNtupleDColumn(0, 4, Wavelength);
        analysisManager -> AddNtupleRow(0);
        
        analysisManager -> FillNtupleIColumn(2, 0, Event);
        analysisManager -> FillNtupleDColumn(2, 1, posDetector[0]);
        analysisManager -> FillNtupleDColumn(2, 2, posDetector[1]);
        analysisManager -> FillNtupleDColumn(2, 3, posDetector[2]);
        analysisManager -> AddNtupleRow(2);
    }

    if (arguments == 4)
    {
        digits = 5; defaultDecimals = G4cout.precision(digits);
        analysisManager -> FillNtupleDColumn(0, 0, posPhoton[0]);
        analysisManager -> FillNtupleDColumn(0, 1, posPhoton[1]);
        if (Energy > 0.0) {analysisManager -> FillNtupleDColumn(0, 2, Energy);}
        analysisManager -> AddNtupleRow(0);
        G4cout.precision(defaultDecimals);
    }

    if (arguments == 5)
    {
        digits = 5; defaultDecimals = G4cout.precision(digits);
        analysisManager -> FillNtupleDColumn(0, 0, posPhoton[0]);
        analysisManager -> FillNtupleDColumn(0, 1, posPhoton[1]);
        analysisManager -> AddNtupleRow(0);
        G4cout.precision(defaultDecimals);
    }

    return true;
}