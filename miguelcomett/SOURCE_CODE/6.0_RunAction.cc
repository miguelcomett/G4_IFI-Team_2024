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
    if (isMaster && fRun) { 
        fRun -> EndOfRun(); }

    G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();
    if (analysisManager) 
        analysisManager -> Write();
        analysisManager -> CloseFile();    
}