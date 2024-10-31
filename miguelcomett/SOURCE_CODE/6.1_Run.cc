#include "6.0_RunAction.hh"
#include "6.1_Run.hh"

extern int arguments;

Run::Run(){}
Run::~Run(){}

void Run::SetPrimary(G4ParticleDefinition * particle, G4double energy)
{ 
    link_ParticleDefinition = particle; 
    link_Energy = energy;
}

void Run::CountProcesses(G4String procName) 
{
    if (arguments == 3)
    {
        std::map <G4String, G4int> ::iterator it = fProcCounter.find(procName);
        if ( it == fProcCounter.end()) {fProcCounter[procName] = 1;} else {fProcCounter[procName]++;}
    }
} 

void Run::EndOfRun()
{
    if (arguments == 3) 
    {
        G4int prec = 5; 
        G4int dfprec = G4cout.precision(prec);

        const MyDetectorConstruction * detectorConstruction = static_cast < const MyDetectorConstruction *> (G4RunManager::GetRunManager() -> GetUserDetectorConstruction());     

        G4String particleName = link_ParticleDefinition -> GetParticleName();    
        G4Material * material = detectorConstruction -> GetMaterial();
        G4double density      = material  -> GetDensity();
        G4double Thickness    = detectorConstruction -> GetThickness();
            
        G4cout << "\n ========================== run summary ======================== \n";

        G4cout << "\n The run is: " << numberOfEvent << " " << particleName << " of "
            << G4BestUnit(link_Energy, "Energy") << " through " 
            << G4BestUnit(Thickness, "Length") << " of "
            << material -> GetName() << " (density: " 
            << G4BestUnit(density, "Volumic Mass") << ")" << G4endl;

        G4int totalCount = 0;
        G4int survive = 0;  
        G4cout << "\n Process calls frequency --->";
        std::map < G4String, G4int >::iterator it;  
        
        for (it = fProcCounter.begin(); it != fProcCounter.end(); it++) 
        {
            G4String procName = it -> first;
            G4int    count    = it -> second;
            totalCount += count; 
            G4cout << "\t" << procName << " = " << count;
            if (procName == "Transportation") survive = count;
        }
        G4cout << G4endl;

        if (totalCount == 0) { G4cout.precision(dfprec);   return;};  

        G4double ratio = double(survive)/totalCount;

        G4cout << "\n Nb of incident particles unaltered after "
                << G4BestUnit(Thickness,"Length") << " of "
                << material -> GetName() << " : " << survive 
                << " over " << totalCount << " incident particles."
                << "  Ratio = " << 100 * ratio << " %" << G4endl;
        
        // if (ratio == 0.0) return;
        
        G4double crossSection = - std::log(ratio)/Thickness;     
        G4double Coefficient = crossSection/density;
        
        G4cout << " ---> CrossSection per volume:\t" << crossSection*cm << " cm^-1 "
               << "\tCrossSection per mass: " << G4BestUnit(Coefficient, "Surface/Mass")
               << G4endl;
        
        Coefficient = Coefficient * g / cm2;
        link_Energy = link_Energy / keV;
        G4AnalysisManager * analysisManager = G4AnalysisManager::Instance();

        analysisManager -> FillNtupleDColumn(0, 0, Coefficient);
        analysisManager -> FillNtupleDColumn(0, 1, link_Energy);
        analysisManager -> FillNtupleDColumn(0, 2, survive);
        analysisManager -> AddNtupleRow(0);
       
        fProcCounter.clear(); // remove all contents in fProcCounter 
        G4cout.precision(dfprec); //restore default format
    }

    G4cout << "\n ===============================================================\n";
    G4cout << "\n ";
}

void Run::Merge(const G4Run * run)
{
    if (arguments == 3)
    {
        const Run * localRun = static_cast <const Run*> (run);

        // pass information about primary particle
        link_ParticleDefinition = localRun -> link_ParticleDefinition;
        link_Energy = localRun -> link_Energy;
            
        std::map<G4String,G4int>::const_iterator it;
        for (it  = localRun -> fProcCounter.begin(); 
            it != localRun -> fProcCounter.end(); ++it) 
        {
            G4String procName = it -> first;
            G4int localCount  = it -> second;

            if ( fProcCounter.find(procName) == fProcCounter.end()) 
            {fProcCounter[procName] = localCount;} else {fProcCounter[procName] += localCount;}         
        }

        G4Run::Merge(run);
    }
} 