#include "6.0_RunAction.hh"

MyRunAction::MyRunAction()
{
    new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
    new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
    new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
    new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

    G4AccumulableManager * accumulableManager = G4AccumulableManager::Instance();
    accumulableManager -> RegisterAccumulable(fEdep);

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    analysisManager -> SetDefaultFileType("root");
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

        analysisManager -> CreateNtuple("Run Summary", "Run Summary");
        analysisManager -> CreateNtupleDColumn("Number_of_Photons");
        analysisManager -> CreateNtupleDColumn("Initial_Energy_keV");
        analysisManager -> CreateNtupleDColumn("Sample_Mass_g");
        analysisManager -> CreateNtupleDColumn("EDep_Value_PeV");
        analysisManager -> CreateNtupleDColumn("Radiation_Dose_mSv");
        analysisManager -> FinishNtuple(1);

        analysisManager -> CreateNtuple("EDep Sample", "EDep Sample");
        analysisManager -> CreateNtupleDColumn("EDep_Spectra");
        analysisManager -> FinishNtuple(2);
    }

    if (arguments == 5)
    {
        analysisManager -> CreateNtuple("Hits", "Hits");
        analysisManager -> CreateNtupleDColumn("x_ax");
        analysisManager -> CreateNtupleDColumn("y_ax");
        analysisManager -> FinishNtuple(0);

        analysisManager -> CreateNtuple("Run Summary", "Run Summary");
        analysisManager -> CreateNtupleDColumn("Number_of_Photons");
        analysisManager -> CreateNtupleDColumn("Initial_Energy_keV");
        analysisManager -> CreateNtupleDColumn("Sample_Mass_g");
        analysisManager -> CreateNtupleDColumn("EDep_Value_PeV");
        analysisManager -> CreateNtupleDColumn("Radiation_Dose_mSv");
        analysisManager -> FinishNtuple(1);
    }
}

MyRunAction::~MyRunAction(){}

G4Run * MyRunAction::GenerateRun() { customRun = new Run(); return customRun; }

void MyRunAction::AddEdep(G4double edep) { fEdep += edep; }

void MyRunAction::BeginOfRunAction(const G4Run * thisRun)
{
    G4AccumulableManager * accumulableManager = G4AccumulableManager::Instance();
    accumulableManager -> Reset();

    primaryGenerator = static_cast < const MyPrimaryGenerator *> (G4RunManager::GetRunManager() -> GetUserPrimaryGeneratorAction()); 
    if (primaryGenerator && primaryGenerator -> GetParticleGun()) 
    {
        particle = primaryGenerator -> GetParticleGun() -> GetParticleDefinition();
        energy = primaryGenerator -> GetParticleGun() -> GetParticleEnergy();
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
    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    
    G4AccumulableManager * accumulableManager = G4AccumulableManager::Instance();
    accumulableManager -> Merge();
    
    if (isMaster && arguments != 3) 
    { 

        detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());   
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
        G4cout << "--> Radiation dose : " << G4BestUnit(radiationDose, "Dose") << G4endl;
        G4cout << G4endl;

        std::tm * now_tm_0 = std::localtime(&now_start);
        G4cout << "Start time: " << std::put_time(now_tm_0, "%H:%M:%S") << "    Date: " << std::put_time(now_tm_0, "%d-%m-%Y") << G4endl;
        
        std::tm * now_tm_1 = std::localtime(&now_end);
        G4cout << "Ending time: " << std::put_time(now_tm_1, "%H:%M:%S") << "   Date: " << std::put_time(now_tm_1, "%d-%m-%Y") << G4endl;
        
        G4cout << "Total simulation time: " << G4BestUnit(durationInSeconds, "Time") << G4endl;
        G4cout << "==========================================" << G4endl;
        G4cout << G4endl;
    }
    
    if (arguments == 4 || arguments == 5) 
    {   
        primaryEnergy = primaryEnergy / keV;
        totalMass = totalMass / kg;
        TotalEnergyDeposit = TotalEnergyDeposit / MeV;
        radiationDose = radiationDose / microgray;

        analysisManager -> FillNtupleDColumn(1, 0, numberOfEvents);
        analysisManager -> FillNtupleDColumn(1, 1, primaryEnergy);
        analysisManager -> FillNtupleDColumn(1, 2, totalMass);
        analysisManager -> FillNtupleDColumn(1, 3, TotalEnergyDeposit);
        analysisManager -> FillNtupleDColumn(1, 4, radiationDose);
        analysisManager -> AddNtupleRow(1);
    }

    if (isMaster) {customRun -> EndOfRun();}

    analysisManager -> Write();
    analysisManager -> CloseFile();
    
    if (isMaster) {MergeRootFiles();}

    //std::string currentPath = std::filesystem::current_path().string();
    //G4cout << "Current working directory: " << currentPath << G4endl;
}

void MyRunAction::MergeRootFiles()
{
    G4cout << G4endl;
    G4cout << "============= Merging Process ============" << G4endl;

    TFileMerger merger;
    merger.SetFastMethod(true);

    // Obtener el directorio actual (donde está el ejecutable, probablemente en "build/Release")
    std::string currentPath = std::filesystem::current_path().string();

    // Navegar al directorio superior y luego a "ROOT"
    //std::string rootDirectory = std::filesystem::path(currentPath).parent_path().string() + "\\ROOT\\";

    #ifdef __APPLE__
        std::string rootDirectory = std::filesystem::path(currentPath).string() + "/ROOT/";
    #else
        std::string rootDirectory = std::filesystem::path(currentPath).parent_path().string() + "/ROOT/";
    #endif

    // Crear la subcarpeta "Output" dentro de ROOT si no existe
    std::string outputDirectory = rootDirectory + "Output/";
    if (!std::filesystem::exists(outputDirectory)) 
    {
        std::filesystem::create_directory(outputDirectory);
        G4cout << G4endl;
        G4cout << "-> Output folder created at: " << outputDirectory << G4endl;
    }

    // Iterar sobre los archivos en el directorio ROOT y agregar archivos .root al merger
    for (const auto & entry : std::filesystem::directory_iterator(rootDirectory)) 
    {
        if (entry.is_regular_file() && entry.path().extension() == ".root") 
        {
            std::string filePath = entry.path().string();
            merger.AddFile(filePath.c_str());
            G4cout << "-> Added file: " << filePath << G4endl;

            std::filesystem::remove(entry.path()); // Eliminar el archivo despues de agregarlo al merger
            G4cout << "-> Deleted file: " << filePath << G4endl;
            G4cout << G4endl;
        }
    }

    // G4cout << G4endl;

    std::string mergedFileName = outputDirectory + "merged_output.root"; // Nombre del archivo final fusionado en la subcarpeta Output
    merger.OutputFile(mergedFileName.c_str());

    if (merger.Merge()) 
    {
        // G4cout << G4endl;
        G4cout << "Successfully merged ROOT files into: " << G4endl;
        G4cout << mergedFileName << G4endl;
        SingleData(mergedFileName);
        // G4cout << "Current working directory: " << rootDirectory << G4endl;
    }
    else { G4cout << "Error during ROOT file merging!" << G4endl; }

    G4cout << "==========================================" << G4endl;
    G4cout << G4endl;
}

//void MyRunAction::SingleData(const std::string & mergedFileName)
//{
//    TFile * mergedFile = TFile::Open(mergedFileName.c_str(), "UPDATE");
//    if (!mergedFile || mergedFile -> IsZombie()) 
//    {
//        G4cout << "Error: Unable to open the merged file: " << mergedFileName << G4endl;
//        return;
//    }
//
//    TTree * tree = dynamic_cast<TTree*>(mergedFile -> Get("Run Summary")); // Obtener el arbol del archivo
//    if (!tree) 
//    {
//        G4cout << "Error: Tree 'Run Summary' not found in the merged file." << G4endl;
//        mergedFile -> Close();
//        return;
//    }
//
//    double numberOfPhotons, initialEnergy, sampleMass, edepValue, radiationDose;  // Variables para almacenar los datos de las columnas
//
//    // Configura las ramas
//    tree -> SetBranchAddress("Number_of_Photons",  & numberOfPhotons);
//    tree -> SetBranchAddress("Initial_Energy_keV", & initialEnergy);
//    tree -> SetBranchAddress("Sample_Mass_g",      & sampleMass);
//    tree -> SetBranchAddress("EDep_Value_PeV",     & edepValue);
//    tree -> SetBranchAddress("Radiation_Dose_mSv", & radiationDose);
//
//    // Inicializa las variables para los valores maximos
//    double maxNumberOfPhotons = -DBL_MAX;
//    double maxInitialEnergy   = -DBL_MAX;
//    double maxSampleMass      = -DBL_MAX;
//    double maxEdepValue       = -DBL_MAX;
//    double maxRadiationDose   = -DBL_MAX;
//
//    Long64_t maxEntryIndex = -1; // Para almacenar el indice de la entrada con el valor maximo
//
//    TTree * newTree = tree -> CloneTree(0); // Creamos un nuevo arbol vacio para almacenar las entradas validas
//
//    for (Long64_t i = 0; i < tree->GetEntries(); ++i) // Itera sobre todas las entradas del arbol
//    {
//        tree -> GetEntry(i); // Leer la entrada
//
//        // Comprobar si alguno de los valores es cero y, si es asi, no agregarlo
//        if (numberOfPhotons == 0 || initialEnergy == 0 || sampleMass == 0 || edepValue == 0 || radiationDose == 0) 
//        {
//            continue; // Si alguno de los valores es cero, pasar a la siguiente entrada
//        }
//        
//        if (numberOfPhotons > maxNumberOfPhotons) // Comparar y actualizar los valores maximos
//        {
//            maxNumberOfPhotons = numberOfPhotons;
//            maxInitialEnergy = initialEnergy;
//            maxSampleMass = sampleMass;
//            maxEdepValue = edepValue;
//            maxRadiationDose = radiationDose;
//            maxEntryIndex = i; // Guardamos el indice de la entrada con los valores maximos
//        }
//
//        newTree -> Fill(); // Rellenar el nuevo arbol con las entradas validas
//    }
//
//    if (maxEntryIndex == -1) 
//    {
//        G4cout << "Error: No valid entries found in the tree." << G4endl;
//        mergedFile -> Close();
//        return;
//    }
//    
//    TTree * maxTree = tree -> CloneTree(0); // Crear un nuevo arbol vacio con la misma estructura
//
//    tree -> GetEntry(maxEntryIndex); // Obtener la entrada con el valor maximo
//
//    // Establecer las ramas del nuevo arbol con los valores maximos
//    maxTree -> SetBranchAddress("Number_of_Photons",  & maxNumberOfPhotons);
//    maxTree -> SetBranchAddress("Initial_Energy_keV", & maxInitialEnergy);
//    maxTree -> SetBranchAddress("Sample_Mass_g",      & maxSampleMass);
//    maxTree -> SetBranchAddress("EDep_Value_PeV",     & maxEdepValue);
//    maxTree -> SetBranchAddress("Radiation_Dose_mSv", & maxRadiationDose);
//
//    // Llenar el nuevo arbol con solo la entrada maxima
//    maxTree -> Fill();
//    
//    maxTree -> Write("Run Summary", TObject::kOverwrite); // Sobrescribir el arbol original con el nuevo arbol que solo tiene la entrada maxima
//
//    mergedFile -> Close();
//    G4cout << "Zero entries have been removed, and only the maximum entry has been kept in the merged ROOT file: " << mergedFileName << G4endl;
//}

void MyRunAction::SingleData(const std::string& mergedFileName)
{
    G4cout << "Opening merged file: " << mergedFileName << G4endl;
    TFile* mergedFile = TFile::Open(mergedFileName.c_str(), "UPDATE");
    if (!mergedFile || mergedFile -> IsZombie())
    {
        G4cout << "Error: Unable to open the merged file: " << mergedFileName << G4endl;
        return;
    }

    G4cout << "Attempting to retrieve 'Run Summary' tree from merged file..." << G4endl;
    TTree* tree = dynamic_cast<TTree*>(mergedFile->Get("Run Summary")); // Obtener el arbol del archivo
    if (!tree)
    {
        G4cout << "Error: Tree 'Run Summary' not found in the merged file." << G4endl;
        mergedFile->Close();
        return;
    }

    G4cout << "Setting branch addresses..." << G4endl;
    double numberOfPhotons, initialEnergy, sampleMass, edepValue, radiationDose;
    tree->SetBranchAddress("Number_of_Photons", &numberOfPhotons);
    tree->SetBranchAddress("Initial_Energy_keV", &initialEnergy);
    tree->SetBranchAddress("Sample_Mass_g", &sampleMass);
    tree->SetBranchAddress("EDep_Value_PeV", &edepValue);
    tree->SetBranchAddress("Radiation_Dose_mSv", &radiationDose);

    G4cout << "Branch addresses set. Initializing maximum values..." << G4endl;
    double maxNumberOfPhotons = -DBL_MAX;
    double maxInitialEnergy   = -DBL_MAX;
    double maxSampleMass      = -DBL_MAX;
    double maxEdepValue       = -DBL_MAX;
    double maxRadiationDose   = -DBL_MAX;

    Long64_t maxEntryIndex = -1;
    TTree* newTree = tree->CloneTree(0);

    G4cout << "Starting to iterate over entries..." << G4endl;
    for (Long64_t i = 0; i < tree->GetEntries(); ++i)
    {
        tree->GetEntry(i);

        // Debugging: Print entry values
        G4cout << "Entry " << i << " - Photons: " << numberOfPhotons
            << ", Initial Energy: " << initialEnergy
            << ", Sample Mass: " << sampleMass
            << ", Edep Value: " << edepValue
            << ", Radiation Dose: " << radiationDose << G4endl;

        if (numberOfPhotons == 0 || initialEnergy == 0 || sampleMass == 0 || edepValue == 0 || radiationDose == 0)
        {
            G4cout << "Skipping entry " << i << " due to zero value(s)." << G4endl;
            continue;
        }

        if (numberOfPhotons > maxNumberOfPhotons)
        {
            G4cout << "New maximum found at entry " << i << G4endl;
            maxNumberOfPhotons = numberOfPhotons;
            maxInitialEnergy = initialEnergy;
            maxSampleMass = sampleMass;
            maxEdepValue = edepValue;
            maxRadiationDose = radiationDose;
            maxEntryIndex = i;
        }

        newTree->Fill();
    }

    if (maxEntryIndex == -1)
    {
        G4cout << G4endl;
        G4cout << "Error: No valid entries found in the tree." << G4endl;
        mergedFile->Close();
        return;
    }

    G4cout << "Creating tree for maximum entry only..." << G4endl;
    TTree* maxTree = tree->CloneTree(0);
    tree->GetEntry(maxEntryIndex);

    maxTree->SetBranchAddress("Number_of_Photons", &maxNumberOfPhotons);
    maxTree->SetBranchAddress("Initial_Energy_keV", &maxInitialEnergy);
    maxTree->SetBranchAddress("Sample_Mass_g", &maxSampleMass);
    maxTree->SetBranchAddress("EDep_Value_PeV", &maxEdepValue);
    maxTree->SetBranchAddress("Radiation_Dose_mSv", &maxRadiationDose);

    maxTree->Fill();
    maxTree->Write("Run Summary", TObject::kOverwrite);

    mergedFile->Close();
    G4cout << "Process completed successfully. Zero entries removed, maximum entry retained in: " << mergedFileName << G4endl;
}
