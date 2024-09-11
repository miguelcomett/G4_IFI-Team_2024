
#include "ActionInit.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "RunAction.hh"

namespace G4_PCM {

	void ActionInit::Build() const {
		// Multithreaded

		// set the Geant4 actions
	
		
		PrimaryGeneratorAction *generator = new PrimaryGeneratorAction(); 
		SetUserAction(generator); 
		
		RunAction *runAction = new RunAction(); 
		SetUserAction(runAction); 
		
		EventAction *eventAction = new EventAction(runAction); 
		SetUserAction(eventAction); 
		
		SteppingAction *steppingAction = new SteppingAction(eventAction); 
		SetUserAction(steppingAction); 

	};

	void ActionInit::BuildForMaster() const {
		// Sequential

		SetUserAction(new RunAction());

	}

}
