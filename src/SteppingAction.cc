
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "G4AnalysisManager.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"

namespace G4_PCM {
	SteppingAction::SteppingAction(EventAction* eventAction) {
		// Get our detector volume 
		feventAction = eventAction; 
	}

	SteppingAction::~SteppingAction() {

	}

	void SteppingAction::UserSteppingAction(const G4Step* step) {
	
		const auto *detConstruction = static_cast<const DetectorConstruction*>(
			G4RunManager::GetRunManager()->GetUserDetectorConstruction()
			);
		fGammaDetector = detConstruction->GetGammaDetector();
		fBoxDestroyer = detConstruction->GetBoxDestroyer();

		G4Track* track = step->GetTrack();

		// Register hit if the step is inside the tracking volume
		// end here if the particle isn't in the detector
		// get volume of the current step
		G4LogicalVolume* volume
			= step->GetPreStepPoint()->GetTouchableHandle()
			->GetVolume()->GetLogicalVolume();

		if (volume != fGammaDetector) { return; }
		if (volume == fBoxDestroyer) { track->SetTrackStatus(fStopAndKill); }

		if (track->GetDynamicParticle()->GetDefinition()->GetParticleName() == "gamma")
		{

			// If it's the first step in the volume, save the position. 
			if (step->IsFirstStepInVolume()) {
				feventAction->SetPosition(step->GetPreStepPoint()->GetPosition());
			}

			// Register all the energy to the eventAction while it's in the detector.
			// feventAction->AddEnergy(step->GetTotalEnergyDeposit());
			feventAction->AddEnergy(step->GetPreStepPoint()->GetKineticEnergy());
			feventAction->AddWeight(step->GetPreStepPoint()->GetWeight()); // Mantener el valor como G4double
		}
	}
	
}
