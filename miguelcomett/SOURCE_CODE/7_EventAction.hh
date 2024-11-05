#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

#include "G4Threading.hh"

#include "6.0_RunAction.hh"
#include "6.1_Run.hh"


class MyEventAction : public G4UserEventAction
{
    public:
        
        MyEventAction(MyRunAction * runAction);
        ~MyEventAction();

        virtual void BeginOfEventAction(const G4Event *);
        virtual void EndOfEventAction  (const G4Event *);

        void AddEDep(G4double EDep);

    private:
        
        MyRunAction * fRunAction = nullptr;
        
        G4double fEDep;
        G4double EDepBuffer;

        G4int numberOfEvents;
        G4int eventID;
        G4int primaryEnergy;
        G4double totalMass, radiationDose, TotalEnergyDeposit = 0.0;
        G4int currentEvent2;

        const G4double milligray = 1.0e-3*gray;
};

#endif