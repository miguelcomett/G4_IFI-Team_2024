#ifndef EventAction_hh
#define EventAction_hh

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

#include "G4Threading.hh"

#include "6.0_RunAction.hh"
#include "6.1_Run.hh"

extern int arguments;

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
        
        G4double fEDep, EDep_keV;
};

#endif