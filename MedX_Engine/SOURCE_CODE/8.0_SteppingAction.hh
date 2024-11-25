#ifndef SteppingAction_hh
#define SteppingAction_hh

#include <algorithm>

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"

#include "3.0_DetectorConstruction.hh"
#include "6.1_Run.hh"
#include "7.0_EventAction.hh"

extern int arguments;

class SteppingAction : public G4UserSteppingAction
{
    public:

        SteppingAction(EventAction * eventAction);
        ~ SteppingAction();

        virtual void UserSteppingAction(const G4Step *);
    
    private:

        G4double worldMaxX, worldMinX, worldMaxY, worldMinY, worldMaxZ, worldMinZ;

        EventAction * fEventAction;
        G4LogicalVolume * scoringVolume, * Volume, * currentLogicVolume;
        G4StepPoint * endPoint;

        G4UserLimits * stepLimit;
        
        G4String processName;
        G4double EDep, minStepSize, stepLength;

        G4VPhysicalVolume * currentPhysVolume;
        const DetectorConstruction * detectorConstruction;

        struct ParticleData {
            G4ThreeVector lastPosition;
            int stuckStepCount;
        };
        std::map<G4int, ParticleData> stuckParticles;
};

#endif