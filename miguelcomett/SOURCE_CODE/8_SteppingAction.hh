#ifndef SteppingAction_hh
#define SteppingAction_hh

#include <algorithm>

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"

#include "3.0_DetectorConstruction.hh"
#include "6.1_Run.hh"
#include "7_EventAction.hh"

class MySteppingAction : public G4UserSteppingAction
{
    public:
        MySteppingAction(MyEventAction * eventAction);
        ~ MySteppingAction();

        virtual void UserSteppingAction(const G4Step *);
    
    private:
        MyEventAction * fEventAction;
        G4bool noSecondaryGamma, noSecondaryElectrons;
        G4double EDep;
};

#endif