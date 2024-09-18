#ifndef G4_PCM_PRIMARY_GENERATOR_MESSENGER_H
#define G4_PCM_PRIMARY_GENERATOR_MESSENGER_H 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"

namespace G4_PCM
{
    class PrimaryGeneratorAction;
    class PrimaryGeneratorMessenger : public G4UImessenger
    {
    public:

        PrimaryGeneratorMessenger(PrimaryGeneratorAction* gun);
        ~PrimaryGeneratorMessenger() override;
        void SetNewValue(G4UIcommand* command, G4String newValue) override;
    private:
        PrimaryGeneratorAction* fGun;
        G4UIcmdWithADoubleAndUnit* fPgunCmd; 
        G4UIcmdWithADouble *fPgunAngleCmd;
    };
}

#endif
