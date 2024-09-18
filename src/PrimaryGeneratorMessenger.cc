#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"

namespace G4_PCM
{
    PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* gun)
        : fGun(gun)
    {
    	// Comando para la posición
        fPgunCmd = new G4UIcmdWithADoubleAndUnit("/Pgun/Z", this);
        fPgunCmd->SetGuidance("Set the source z position.");
        fPgunCmd->SetParameterName("zpos", true);
        fPgunCmd->SetDefaultUnit("cm");
        
        // Comando para el ángulo
        fPgunAngleCmd = new G4UIcmdWithADouble("/Pgun/Angle", this);
        fPgunAngleCmd->SetGuidance("Set the source angle.");
        fPgunAngleCmd->SetParameterName("angle", true);
        //fPgunAngleCmd->SetDefaultUnit("deg");
    }

    PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
    {
        delete fPgunCmd;
        delete fPgunAngleCmd; 
    }

    void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
    {
        if (command == fPgunCmd)
        {
            G4double zpos = fPgunCmd->GetNewDoubleValue(newValue);
            G4cout << "Command received: /Pgun/Z " << zpos << G4endl;
            fGun->SetGunZpos(zpos);
        }
        else if (command == fPgunAngleCmd)
        {
            G4double angle = fPgunAngleCmd->GetNewDoubleValue(newValue);
            G4cout << "Command received: /Pgun/Angle " << angle << G4endl;
            fGun->SetGunAngle(angle);
        }
    }
}
