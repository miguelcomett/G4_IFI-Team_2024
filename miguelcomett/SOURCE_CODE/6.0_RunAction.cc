#include "6.0_RunAction.hh"

extern int arguments;

MyRunAction::MyRunAction()
{
    if (arguments == 1 || arguments == 2)
    {dataTitle = "Energy_Deposition_keV";}
    else if (arguments == 4)
    {dataTitle = "Photons'_Energy_keV";}

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

    if (arguments == 1 || arguments == 2 || arguments == 4)
    {
        analysisManager -> CreateNtuple("Photons", "Photons");
        analysisManager -> CreateNtupleIColumn("Event_Count");
        analysisManager -> CreateNtupleDColumn("X_axis");
        analysisManager -> CreateNtupleDColumn("Y_axis");
        analysisManager -> CreateNtupleDColumn("Z_axis");
        analysisManager -> CreateNtupleDColumn("Photons'_Wavelengths_nm");
        analysisManager -> CreateNtupleDColumn(dataTitle);
        analysisManager -> FinishNtuple(0);
    }

    if (arguments == 1 || arguments == 2)
    {
        analysisManager -> CreateNtuple("Hits", "Hits");
        analysisManager -> CreateNtupleIColumn("Event_Count");
        analysisManager -> CreateNtupleDColumn("X_Detectors");
        analysisManager -> CreateNtupleDColumn("Y_Detectors");
        analysisManager -> CreateNtupleDColumn("Z_Detectors");
        analysisManager -> FinishNtuple(1);
    }

    if (arguments == 3)
    {
        analysisManager -> CreateNtuple("Transportation", "Transportation");
        analysisManager -> CreateNtupleDColumn("Mass_Attenuation");
        analysisManager -> CreateNtupleDColumn("Energy_keV");
        analysisManager -> CreateNtupleDColumn("Ratio");
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
    
    if (primaryGenerator) 
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

void MyRunAction::EndOfRunAction(const G4Run *)
{
    if (isMaster) { fRun -> EndOfRun(); }

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    analysisManager -> Write();
    analysisManager -> CloseFile();    
}