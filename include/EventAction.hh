#ifndef G4_PCM_EVENT_ACTION_HH
#define G4_PCM_EVENT_ACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

// Forward declaration of RunAction
namespace G4_PCM {
    class RunAction;  // Forward declaration
}

namespace G4_PCM {

    class EventAction : public G4UserEventAction {
    public:
        EventAction(RunAction*);
        ~EventAction() override = default;

        void BeginOfEventAction(const G4Event* anEvent) override;
        void EndOfEventAction(const G4Event* anEvent) override;

        void AddEnergy(G4double energy);
        void SetPosition(G4ThreeVector pos);

        void Print();

        // Method to get the count of ntuple registrations
        static int GetNtupleRegistrationCount();
        static void ResetNtupleRegistrationCount(); // Add this method

    private:
        G4double fEnergy;
        G4ThreeVector fPosition;

        // Static variable to count the number of ntuple registrations
        static int fNtupleRegistrationCount;
    };

}

#endif // !G4_PCM_EVENT_ACTION_HH
