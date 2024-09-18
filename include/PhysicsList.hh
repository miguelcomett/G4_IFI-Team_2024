#ifndef G4_PCM_PHYSICS_LIST_H
#define G4_PCM_PHYSICS_LIST_H 1

#include "G4VModularPhysicsList.hh"

namespace G4_PCM
{
	class PhysicsList : public G4VModularPhysicsList
	{
	public:
		PhysicsList();
		virtual ~PhysicsList();

		// Mandatory methods to override;
		virtual void ConstructParticle() override;
		virtual void ConstructProcess() override;
	};
}

#endif