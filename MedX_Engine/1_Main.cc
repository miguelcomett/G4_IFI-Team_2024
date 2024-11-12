#include <iostream>
#include <ctime> 
#include <chrono>
#include <iomanip>

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UIManager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4RunManagerFactory.hh"

#include "2.0_PhysicsList.hh"
#include "3.0_DetectorConstruction.hh"
#include "4.0_ActionInitialization.hh"

int arguments = 0;

int main(int argc, char** argv)
{
    arguments = (argc);

    #ifdef __APPLE__

        G4RunManager * runManager;
        // G4cout << G4endl; G4cout << "~~~~~~~~~~~~ Running on macOS ~~~~~~~~~~~~" << G4endl;

        if (argc == 1) 
        {
            // G4cout << "===== Running in Single-threaded mode ====" << G4endl; G4cout << G4endl;
            runManager = new G4RunManager();
        } 
        else 
        {
            // G4cout << "===== Running in Multi-threaded mode =====" << G4endl;
            runManager = new G4MTRunManager();
        }
    #endif

    #ifdef _WIN32
        // std::cout << std::endl; std::cout << "~~~~~~~~~~~~ Running on Windows ~~~~~~~~~~~" << std::endl;
        auto * runManager =G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    #endif

    long seed = std::time(nullptr);
    CLHEP::HepRandom::setTheSeed(seed);

    runManager -> SetUserInitialization(new MyDetectorConstruction);
    runManager -> SetUserInitialization(new MyPhysicsList);
    runManager -> SetUserInitialization(new MyActionInitialization); 

    G4UImanager * UImanager = G4UImanager::GetUIpointer();
    
    if(argc == 1)
    {
        G4VisManager * visManager = new G4VisExecutive("quiet");
        visManager -> Initialize();

        G4UIExecutive * UI = nullptr;
        UI = new G4UIExecutive(argc, argv);
        UImanager -> ApplyCommand("/control/execute Visualization.mac");
        UI -> SessionStart();
        
        delete UI;
        delete visManager;
    }
    else
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager -> ApplyCommand(command + fileName);
    }

    delete runManager;
}

// std::chrono::system_clock::time_point simulationStartTime;
// simulationStartTime = std::chrono::system_clock::now();
// auto now = std::chrono::system_clock::now();
// std::time_t now_c = std::chrono::system_clock::to_time_t(simulationStartTime);
// std::tm * now_tm = std::localtime(&now_c);
// G4cout << G4endl; 
// G4cout << ".........................................." << G4endl;
// G4cout << "Start time: " << std::put_time(now_tm, "%H:%M:%S") << "    Date: " << std::put_time(now_tm, "%d-%m-%Y") << G4endl;
// G4cout << ".........................................." << G4endl;