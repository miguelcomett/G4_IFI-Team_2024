#include "4.0_ActionInitialization.hh"

MyActionInitialization::MyActionInitialization(MyDetectorConstruction* detector)
    : G4VUserActionInitialization(), fDetector(detector) {}
MyActionInitialization::~MyActionInitialization(){}

void MyActionInitialization::BuildForMaster() const 
{
    MyRunAction * runAction = new MyRunAction();
    SetUserAction(runAction);
}

void MyActionInitialization::Build() const
{
    // Pasar fDetector al constructor
    MyPrimaryGenerator* generator = new MyPrimaryGenerator(fDetector);
    SetUserAction(generator);
    
    MyRunAction * runAction = new MyRunAction();
    SetUserAction(runAction);

    MyEventAction * eventAction = new MyEventAction(runAction);
    SetUserAction(eventAction);

    MySteppingAction * steppingAction = new MySteppingAction(eventAction);
    SetUserAction(steppingAction);

    // Pasar el puntero de DetectorConstruction a PrimaryGeneratorAction
    SetUserAction(new MyPrimaryGenerator(fDetector));
}
