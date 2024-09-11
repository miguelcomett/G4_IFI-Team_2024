#include "PhysicsList.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4OpticalPhysics.hh"

namespace G4_PCM
{
    PhysicsList::PhysicsList() : G4VModularPhysicsList()
    {
        // Remove the default EM physics
        // RegisterPhysics(new G4EmStandardPhysics()); // No longer needed

        // Register the desired EM physics
        RegisterPhysics(new G4EmLivermorePhysics());

        // Register the optical physics
        RegisterPhysics(new G4OpticalPhysics());
    }

    PhysicsList::~PhysicsList() {}

    void PhysicsList::ConstructParticle() {
        // Construct particles here
        G4VModularPhysicsList::ConstructParticle();
    }

    void PhysicsList::ConstructProcess() {
        // Construct processes here
        G4VModularPhysicsList::ConstructProcess();
    }
}
