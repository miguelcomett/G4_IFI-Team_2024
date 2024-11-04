#include "6.0_RunAction.hh"

MyRunAction::MyRunAction()
{
    const G4double milligray = 1.0e-3*gray;
    const G4double microgray = 1.0e-6*gray;
    const G4double nanogray  = 1.0e-9*gray;
    const G4double picogray  = 1.0e-12*gray;

    new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
    new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
    new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
    new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

    G4AccumulableManager * accumulableManager = G4AccumulableManager::Instance();
    accumulableManager -> RegisterAccumulable(fEdep);

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    analysisManager -> SetDefaultFileType("root");
    analysisManager -> SetNtupleMerging(true);
    analysisManager -> SetVerboseLevel(0);

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
        analysisManager -> CreateNtupleDColumn("Detected_Energy_keV");
        analysisManager -> FinishNtuple(0);

        analysisManager -> CreateNtuple("Radiation Dose", "Radiation Dose");
        analysisManager -> CreateNtupleDColumn("Mass");
        analysisManager -> CreateNtupleDColumn("EDep_Sum");
        analysisManager -> CreateNtupleDColumn("Dose");
        analysisManager -> FinishNtuple(1);

        analysisManager -> CreateNtuple("Sample EDep (keV)", "Sample EDep");
        analysisManager -> CreateNtupleDColumn("EDep_Spectra");
        analysisManager -> FinishNtuple(2);
    }

    if (arguments == 5)
    {
        analysisManager -> CreateNtuple("Photons", "Photons");
        analysisManager -> CreateNtupleDColumn("X_axis");
        analysisManager -> CreateNtupleDColumn("Y_axis");
        analysisManager -> FinishNtuple(0);

        analysisManager -> CreateNtuple("Radiation Dose", "Radiation Dose");
        analysisManager -> CreateNtupleDColumn("Mass");
        analysisManager -> CreateNtupleDColumn("EDep_Sum");
        analysisManager -> CreateNtupleDColumn("Dose");
        analysisManager -> FinishNtuple(1);

        analysisManager -> CreateNtuple("prueba", "prueba");
        analysisManager -> CreateNtupleDColumn("aaaa");
        analysisManager -> FinishNtuple(2);
    }
}

MyRunAction::~MyRunAction(){}

G4Run * MyRunAction::GenerateRun() { customRun = new Run(); return customRun; }

void MyRunAction::AddEdep(G4double edep) { fEdep += edep; }

void MyRunAction::BeginOfRunAction(const G4Run * thisRun)
{
    G4AccumulableManager * accumulableManager = G4AccumulableManager::Instance();
    accumulableManager -> Reset();

    const MyPrimaryGenerator * primaryGenerator = static_cast < const MyPrimaryGenerator *> (G4RunManager::GetRunManager() -> GetUserPrimaryGeneratorAction()); 
    if (primaryGenerator && primaryGenerator -> GetParticleGun()) 
    {
        G4ParticleDefinition * particle = primaryGenerator -> GetParticleGun() -> GetParticleDefinition();
        G4double energy                 = primaryGenerator -> GetParticleGun() -> GetParticleEnergy();
        customRun -> SetPrimary(particle, energy);
    }

    runID = thisRun -> GetRunID();
    directory = std::string(ROOT_OUTPUT_DIR);

    if (arguments == 1 || arguments == 2)
        fileName = "/Sim_" + std::to_string(runID);
    if (arguments == 3)
        fileName = "/AttCoeff_" + std::to_string(runID);
    if (arguments == 4)
        fileName = "/Rad_" + std::to_string(runID);
    if (arguments == 5)
        fileName = "/CT_" + std::to_string(runID);

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    analysisManager -> SetFileName(directory + fileName);
    analysisManager -> OpenFile();

    if (isMaster){ simulationStartTime = std::chrono::system_clock::now(); }
}


void MyRunAction::EndOfRunAction(const G4Run * thisRun)
{  
    G4AccumulableManager * accumulableManager = G4AccumulableManager::Instance();
    accumulableManager -> Merge();
            
    if (isMaster && arguments != 3) 
    { 
        const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());   
        std::vector <G4LogicalVolume*> scoringVolumes = detectorConstruction -> GetAllScoringVolumes();
        
        totalMass = 0.0;
        index = 1;

        // G4cout << G4endl;
        // G4cout << "-----------------" << G4endl;
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
        // G4cout << "-----------------" << G4endl;
        
        const Run * currentRun = static_cast<const Run *>(thisRun);
        particleName = currentRun -> GetPrimaryParticleName();
        primaryEnergy = currentRun -> GetPrimaryEnergy();
        numberOfEvents = thisRun -> GetNumberOfEvent();

        // customRun -> EndOfRun();

        TotalEnergyDeposit = fEdep.GetValue();
        radiationDose = TotalEnergyDeposit / totalMass;

        std::time_t now_start = std::chrono::system_clock::to_time_t(simulationStartTime);

        simulationEndTime = std::chrono::system_clock::now();
        std::time_t now_end = std::chrono::system_clock::to_time_t(simulationEndTime);
        
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(simulationEndTime - simulationStartTime);
        durationInSeconds = duration.count() * second;

        G4cout << G4endl;
        G4cout << "============== Run Summary ===============" << G4endl;
        G4cout << "The run is: " << numberOfEvents << " " << particleName << " of "<< G4BestUnit(primaryEnergy, "Energy") << G4endl;
        G4cout << "--> Total mass of sample: " << G4BestUnit(totalMass, "Mass") << G4endl;
        G4cout << "--> Total energy deposition: " << G4BestUnit(TotalEnergyDeposit, "Energy") << G4endl;
        G4cout << "--> Total Radiation dosis : " << G4BestUnit(radiationDose,"Dose") << G4endl;
        G4cout << G4endl;

        std::tm * now_tm_0 = std::localtime(&now_start);
        G4cout << "Start time: " << std::put_time(now_tm_0, "%H:%M:%S") << "    Date: " << std::put_time(now_tm_0, "%d-%m-%Y") << G4endl;
        
        std::tm * now_tm_1 = std::localtime(&now_end);
        G4cout << "Ending time: " << std::put_time(now_tm_1, "%H:%M:%S") << "   Date: " << std::put_time(now_tm_1, "%d-%m-%Y") << G4endl;
        
        G4cout << "Total simulation time: " << G4BestUnit(durationInSeconds, "Time") << G4endl;
        G4cout << "==========================================" << G4endl;
        G4cout << G4endl;
    }
    
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    analysisManager -> Write();
    analysisManager -> CloseFile();    
}

void MyRunAction::fill()
{
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    analysisManager -> FillNtupleDColumn(1, 0, totalMass);
    analysisManager -> FillNtupleDColumn(1, 1, TotalEnergyDeposit);
    analysisManager -> FillNtupleDColumn(1, 2, radiationDose);
    analysisManager -> AddNtupleRow(1);   
}