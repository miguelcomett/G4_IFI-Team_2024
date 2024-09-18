#ifndef ActionInitialization_hh
#define ActionInitialization_hh

#include "G4VUserActionInitialization.hh"

#include "3.0_DetectorConstruction.hh"
#include "5_PrimaryGenerator.hh"
#include "6.0_RunAction.hh"
#include "6.1_Run.hh"
#include "8_SteppingAction.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
    public:
        MyActionInitialization();
        ~MyActionInitialization();

        virtual void Build() const;
        virtual void BuildForMaster() const;
};

#endif