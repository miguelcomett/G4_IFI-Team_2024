#include "7_EventAction.hh"

extern int arguments;

MyEventAction::MyEventAction(MyRunAction * runAction) : fRunAction(runAction) 
{
    fEDep = 0.0;
    new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
}

MyEventAction::~MyEventAction(){}

void MyEventAction::AddEDep(G4double EDep) { fEDep = fEDep + EDep; }

void MyEventAction::BeginOfEventAction(const G4Event * event) 
{
    fEDep = 0.0;
}

void MyEventAction::EndOfEventAction(const G4Event * event) 
{ 
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    
    // fEDep = fEDep / keV;

    const G4Run * thisRun = G4RunManager::GetRunManager() -> GetCurrentRun();
    eventID = event -> GetEventID();
    numberOfEvents = thisRun -> GetNumberOfEventToBeProcessed();

    if ( (arguments == 4 || arguments == 5) && eventID == 0) 
    {  
        G4PrimaryVertex * primaryVertex = event -> GetPrimaryVertex();
        G4PrimaryParticle * primaryParticle = primaryVertex -> GetPrimary();

        G4String particleName = primaryParticle -> GetParticleDefinition() -> GetParticleName();
        G4double primaryEnergy = primaryParticle -> GetKineticEnergy();
        primaryEnergy = primaryEnergy / keV;
        G4int eventNumber = event -> GetEventID();
        
        // G4cout << "Number of events: " << numberOfEvents << G4endl;
        // G4cout << "Primary Particle: " << particleName << G4endl;
        // G4cout << "Primary Energy: " << primaryEnergy << " keV" << G4endl;

        const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());   
        std::vector <G4LogicalVolume*> scoringVolumes = detectorConstruction -> GetAllScoringVolumes();
                    
        G4int index = 1;
        G4double sampleMass;

        for (G4LogicalVolume * volume : scoringVolumes) 
        {
            if (volume) 
            {
                sampleMass = volume -> GetMass();
                // G4cout << "Mass " << index << ": " << G4BestUnit(sampleMass, "Mass") << G4endl;
                totalMass = totalMass + sampleMass;
            }
            index = index + 1;
        }
        // G4cout << "--> Total mass of sample: " << G4BestUnit(totalMass, "Mass") << G4endl;

        // totalMass = totalMass / g;

        analysisManager -> FillNtupleDColumn(1, 0, numberOfEvents);
        analysisManager -> FillNtupleDColumn(1, 1, primaryEnergy);
        analysisManager -> FillNtupleDColumn(1, 2, totalMass/g);
        analysisManager -> AddNtupleRow(1);
    }

    if (arguments == 1 || arguments == 2)
    {
        if (fEDep > 0.0) {analysisManager -> FillNtupleDColumn(1, 0, fEDep);}
        analysisManager -> AddNtupleRow(1);
    }

    // G4cout << "energy: " << G4BestUnit(fEDep, "Energy") << G4endl;

    TotalEnergyDeposit = TotalEnergyDeposit + fEDep;

    if ( (arguments == 4 || arguments == 5) && eventID == numberOfEvents - 1) 
    {
        // TotalEnergyDeposit = TotalEnergyDeposit / PeV;
        radiationDose = TotalEnergyDeposit / totalMass;
        // radiationDose = radiationDose / milligray;

        // G4cout << "Total energy deposit: " << G4BestUnit(TotalEnergyDeposit, "Energy") << G4endl;
        // G4cout << "Radiation dose: " << G4BestUnit(radiationDose, "Dose") << G4endl;

        analysisManager -> FillNtupleDColumn(2, 0, TotalEnergyDeposit);
        analysisManager -> FillNtupleDColumn(2, 1, radiationDose);
        analysisManager -> AddNtupleRow(2);
    }
    
    if (arguments == 4)
    {
        if (fEDep == 0.0) {analysisManager -> FillNtupleDColumn(3, 0, fEDep);}
        analysisManager -> AddNtupleRow(3);
    }

    fRunAction -> AddEdep(fEDep);
}

// G4int Event = G4RunManager::GetRunManager() -> GetCurrentEvent() -> GetEventID();
// G4cout << "Energy deposition (keV): " << fEDep << "  |Event ID: " << Event << G4endl; 