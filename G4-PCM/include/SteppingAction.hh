#ifndef G4_PCM_STEPPING_ACTION_H
#define G4_PCM_STEPPING_ACTION_H 1

#include "G4UserSteppingAction.hh"
#include "G4LogicalVolume.hh"
#include "EventAction.hh"


namespace G4_PCM {


	class SteppingAction : public G4UserSteppingAction {
	public:
		SteppingAction(EventAction* eventAction = nullptr);
		~SteppingAction();

		void UserSteppingAction(const G4Step*) override;

	private:
		G4LogicalVolume* fGammaDetector = nullptr;

		EventAction* feventAction;

	};

}


#endif // !G4_PCM_STEPPING_ACTION_H
