#include "6.0_RunAction.hh"

extern int arguments;

MyRunAction::MyRunAction()
{
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    if (arguments == 1 || arguments == 2)
    {
        analysisManager -> CreateNtuple("Photons", "Photons");
        analysisManager -> CreateNtupleIColumn("Event_Count");
        analysisManager -> CreateNtupleDColumn("X_axis");
        analysisManager -> CreateNtupleDColumn("Y_axis");
        analysisManager -> CreateNtupleDColumn("Z_axis");
        analysisManager -> CreateNtupleDColumn("Photons'_Wavelengths_nm");
        analysisManager -> FinishNtuple(0);

        analysisManager -> CreateNtuple("Energy", "Energy");
        analysisManager -> CreateNtupleDColumn("Energy_Deposition_keV");
        analysisManager -> FinishNtuple(1);

        analysisManager -> CreateNtuple("Hits", "Hits");
        analysisManager -> CreateNtupleIColumn("Event_Count");
        analysisManager -> CreateNtupleDColumn("X_Detectors");
        analysisManager -> CreateNtupleDColumn("Y_Detectors");
        analysisManager -> CreateNtupleDColumn("Z_Detectors");
        analysisManager -> FinishNtuple(2);
    }

    if (arguments == 3)
    {
        analysisManager -> CreateNtuple("Transportation", "Transportation");
        analysisManager -> CreateNtupleDColumn("Mass_Attenuation");
        analysisManager -> CreateNtupleDColumn("Energy_keV");
        analysisManager -> CreateNtupleDColumn("Ratio");
        analysisManager -> FinishNtuple(0);
    }

    if (arguments == 4)
    {
        analysisManager -> CreateNtuple("Photons", "Photons");
        analysisManager -> CreateNtupleDColumn("X_axis");
        analysisManager -> CreateNtupleDColumn("Y_axis");
        analysisManager -> FinishNtuple(0);

        analysisManager -> CreateNtuple("Energy", "Energy");
        analysisManager -> CreateNtupleDColumn("Photons'_Energy_keV");
        analysisManager -> FinishNtuple(1);
    }

    if (arguments == 5)
    {
        analysisManager -> CreateNtuple("Photons", "Photons");
        analysisManager -> CreateNtupleDColumn("X_axis");
        analysisManager -> CreateNtupleDColumn("Y_axis");
        analysisManager -> FinishNtuple(0);
    }
}

MyRunAction::~MyRunAction(){}

void MyRunAction::BeginOfRunAction(const G4Run * run)
{
    const MyPrimaryGenerator * primaryGenerator = static_cast < const MyPrimaryGenerator *> (G4RunManager::GetRunManager() -> GetUserPrimaryGeneratorAction()); 

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    G4int runID = run -> GetRunID();
    std::stringstream strRunID;
    strRunID << runID;
    analysisManager -> OpenFile(std::string(ROOT_OUTPUT_DIR) + "/root" + strRunID.str() + ".root");
    
    if (primaryGenerator && primaryGenerator->GetParticleGun()) 
    {
        G4ParticleDefinition * particle = primaryGenerator -> GetParticleGun() -> GetParticleDefinition();
        G4double energy                 = primaryGenerator -> GetParticleGun() -> GetParticleEnergy();
        fRun -> SetPrimary(particle, energy);
    }
}

G4Run * MyRunAction::GenerateRun()
{ 
    fRun = new Run(); 
    return fRun;
}

void MyRunAction::EndOfRunAction(const G4Run * run)
{  
    if (isMaster && fRun && arguments !=3) 
    { 
        const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());     
        sampleMass = detectorConstruction -> GetScoringVolume() -> GetMass();
        
        const Run * currentRun = static_cast<const Run *>(run);
        particleName = currentRun -> GetPrimaryParticleName();
        primaryEnergy = currentRun -> GetPrimaryEnergy();
        numberOfEvents = run -> GetNumberOfEvent();

        G4cout << G4endl;
        G4cout << "============== Run Summary ===============" << G4endl;
        G4cout << "The run is: " << numberOfEvents << " " << particleName << " of "<< G4BestUnit(primaryEnergy, "Energy") << G4endl;
        G4cout << "--> Mass of sample: " << G4BestUnit(sampleMass, "Mass") << G4endl;
        G4cout << "==========================================" << G4endl;

        fRun -> EndOfRun();

        // Get current time
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm * now_tm = std::localtime(&now_c);

        G4cout << G4endl;
        G4cout << "End of Simulation" << G4endl;
        std::cout << ".........................................." << std::endl;
        std::cout << "Ending time: " << std::put_time(now_tm, "%H:%M:%S") << "    Date: " << std::put_time(now_tm, "%d-%m-%Y") << std::endl;
        std::cout << ".........................................." << std::endl;
        std::cout << std::endl;
    }
    
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    if (analysisManager) 
        analysisManager -> Write();
        analysisManager -> CloseFile();    
}