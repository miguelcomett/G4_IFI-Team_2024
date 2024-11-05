#ifndef SteppingAction_hh
#define SteppingAction_hh

#include <algorithm>

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"

#include "3.0_DetectorConstruction.hh"
#include "6.1_Run.hh"
#include "7_EventAction.hh"

extern int arguments;

class MySteppingAction : public G4UserSteppingAction
{
    public:

        MySteppingAction(MyEventAction * eventAction);
        ~ MySteppingAction();

        virtual void UserSteppingAction(const G4Step *);
    
    private:

        MyEventAction * fEventAction;
        G4LogicalVolume * scoringVolume;
        G4LogicalVolume * Volume;
        G4StepPoint * endPoint;
        
        G4String processName;
        G4double EDep;
};

#endif