#ifndef ActionInitialization_hh
#define ActionInitialization_hh

#include "G4VUserActionInitialization.hh"

#include "3.0_DetectorConstruction.hh"
#include "5.0_PrimaryGenerator.hh"
#include "6.0_RunAction.hh"
#include "6.1_Run.hh"
#include "8.0_SteppingAction.hh"

class ActionInitialization : public G4VUserActionInitialization
{
    public:
<<<<<<< Updated upstream
        MyActionInitialization(MyDetectorConstruction* detector);
        ~MyActionInitialization();
=======
        ActionInitialization();
        ~ActionInitialization();
>>>>>>> Stashed changes

        virtual void Build() const;
        virtual void BuildForMaster() const;

        MyDetectorConstruction* fDetector; // Puntero a DetectorConstruction
};

#endif