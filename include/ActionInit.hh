#ifndef G4_PCM_ACTION_INIT_H
#define G4_PCM_ACTION_INIT_H 1

#include "G4VUserActionInitialization.hh"

namespace G4_PCM {
	class ActionInit : public G4VUserActionInitialization {
	public:
		ActionInit() = default;
		~ActionInit() override = default;

		void Build() const override;
		void BuildForMaster() const override;
	
	};
}

#endif // !G4_PCM_ACTION_INIT_H
