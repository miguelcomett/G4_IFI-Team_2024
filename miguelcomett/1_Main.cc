#include <iostream>
#include <ctime> 

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UIManager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4RunManagerFactory.hh"

#include "2_PhysicsList.hh"
#include "3.0_DetectorConstruction.hh"
#include "4_ActionInitialization.hh"

int arguments = 0;

// using namespace std;
// auto* runManager =G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

int main(int argc, char** argv)
{
    arguments = (argc);

    #ifdef __APPLE__

        std::cout << std::endl;
        std::cout << "~~~~~~ Running on macOS ~~~~~~~" << std::endl;
        
        G4RunManager * runManager;

        if (argc == 1) 
        {
            runManager = new G4RunManager();
            G4cout << "===== Running in Single-threaded mode =====" << G4endl;
            G4cout << G4endl;
        } 
        else 
        {
            runManager = new G4MTRunManager();
            G4cout << "====== Running in Multi-threaded mode ======" << G4endl;
            G4cout << G4endl;
        }
        
    #endif

    #ifdef _WIN32
        auto * runManager =G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    #endif

    long seed = std::time(nullptr);
    CLHEP::HepRandom::setTheSeed(seed);

    runManager -> SetUserInitialization(new MyDetectorConstruction);
    runManager -> SetUserInitialization(new MyPhysicsList);
    runManager -> SetUserInitialization(new MyActionInitialization); 

    G4VisManager * visManager = new G4VisExecutive();
    visManager -> Initialize();
    G4UImanager * UImanager = G4UImanager::GetUIpointer();

    if(argc == 1)
    {
        G4UIExecutive * UI = nullptr;
        UI = new G4UIExecutive(argc, argv);
        UImanager -> ApplyCommand("/control/execute Visualization.mac");
        UI -> SessionStart();
        delete UI;
    }
    else
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager -> ApplyCommand(command + fileName);
    }

    delete visManager;
    delete runManager;
}