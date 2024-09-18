#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
namespace G4_PCM
{
	PhysicsList::PhysicsList() : G4VModularPhysicsList() {
		// Get the standard EM particles
		RegisterPhysics(new G4EmStandardPhysics());
		RegisterPhysics(new G4OpticalPhysics());
	}
	PhysicsList::~PhysicsList() {}

	void PhysicsList::ConstructParticle() {
		// Construct particle here
		G4VModularPhysicsList::ConstructParticle();
		
	}
	void PhysicsList::ConstructProcess() {
		// Construct processes here
		G4VModularPhysicsList::ConstructProcess();

	}
}