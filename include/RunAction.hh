#ifndef G4_PCM_RUN_ACTION_HH
#define G4_PCM_RUN_ACTION_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Timer.hh"  // Timing the run
#include "EventAction.hh"  // Include EventAction to reset the ntuple registration count

namespace G4_PCM {

    class RunAction : public G4UserRunAction {
    public:
        RunAction();  // Constructor
        ~RunAction();  // Destructor

        void BeginOfRunAction(const G4Run* aRun) override;  // Begin run action
        void EndOfRunAction(const G4Run* aRun) override;  // End run action

    private:
        G4Timer* timer;  // Timer for tracking elapsed time

        void PrintTime();  // Helper function to print the time
    };

}

#endif // G4_PCM_RUN_ACTION_HH
