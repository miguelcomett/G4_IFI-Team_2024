#include "4.0_ActionInitialization.hh"

<<<<<<< Updated upstream
MyActionInitialization::MyActionInitialization(MyDetectorConstruction* detector)
    : G4VUserActionInitialization(), fDetector(detector) {}
MyActionInitialization::~MyActionInitialization(){}
=======
ActionInitialization::ActionInitialization(){}
ActionInitialization::~ActionInitialization(){}
>>>>>>> Stashed changes

void ActionInitialization::BuildForMaster() const 
{
    RunAction * runAction = new RunAction();
    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
<<<<<<< Updated upstream
    // Pasar fDetector al constructor
    MyPrimaryGenerator* generator = new MyPrimaryGenerator(fDetector);
=======
    PrimaryGenerator * generator = new PrimaryGenerator();
>>>>>>> Stashed changes
    SetUserAction(generator);
    
    RunAction * runAction = new RunAction();
    SetUserAction(runAction);

    EventAction * eventAction = new EventAction(runAction);
    SetUserAction(eventAction);

    SteppingAction * steppingAction = new SteppingAction(eventAction);
    SetUserAction(steppingAction);

    // Pasar el puntero de DetectorConstruction a PrimaryGeneratorAction
    SetUserAction(new MyPrimaryGenerator(fDetector));
}
