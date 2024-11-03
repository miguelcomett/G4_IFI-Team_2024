#include "6.0_RunAction.hh"

extern int arguments;

MyRunAction::MyRunAction()
{
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    analysisManager->SetDefaultFileType("root");
    analysisManager->SetNtupleMerging(true);
    analysisManager->SetVerboseLevel(0);

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


G4Run * MyRunAction::GenerateRun()
{ 
    customRun = new Run(); 
    return customRun;
}


void MyRunAction::BeginOfRunAction(const G4Run * thisRun)
{
    const MyPrimaryGenerator * primaryGenerator = static_cast < const MyPrimaryGenerator *> (G4RunManager::GetRunManager() -> GetUserPrimaryGeneratorAction()); 
    if (primaryGenerator && primaryGenerator -> GetParticleGun()) 
    {
        G4ParticleDefinition * particle = primaryGenerator -> GetParticleGun() -> GetParticleDefinition();
        G4double energy                 = primaryGenerator -> GetParticleGun() -> GetParticleEnergy();
        customRun -> SetPrimary(particle, energy);
    }

    runID = thisRun -> GetRunID();
    directory = std::string(ROOT_OUTPUT_DIR);

    if (arguments == 1 || arguments == 2 || arguments == 4)
        fileName = "/Rad_" + std::to_string(runID);
    if (arguments == 3)
        fileName = "/AttCoeff_" + std::to_string(runID);
    if (arguments == 5)
        fileName = "/CT_" + std::to_string(runID);

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    analysisManager -> SetFileName(directory + fileName);
    analysisManager -> OpenFile();
}


void MyRunAction::EndOfRunAction(const G4Run * thisRun)
{  
    if (isMaster && customRun && arguments !=3) 
    { 
        const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());     
        sampleMass = detectorConstruction -> GetScoringVolume() -> GetMass();
        
        const Run * currentRun = static_cast<const Run *>(thisRun);
        particleName = currentRun -> GetPrimaryParticleName();
        primaryEnergy = currentRun -> GetPrimaryEnergy();
        numberOfEvents = thisRun -> GetNumberOfEvent();

        G4cout << G4endl;
        G4cout << "============== Run Summary ===============" << G4endl;
        G4cout << "The run is: " << numberOfEvents << " " << particleName << " of "<< G4BestUnit(primaryEnergy, "Energy") << G4endl;
        G4cout << "--> Mass of sample: " << G4BestUnit(sampleMass, "Mass") << G4endl;
        G4cout << "==========================================" << G4endl;

        customRun -> EndOfRun();

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