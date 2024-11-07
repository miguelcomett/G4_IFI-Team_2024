#include "5.1_GeneratorMessenger.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(MyPrimaryGenerator* gun)
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

	fParticleModeCmd = new G4UIcmdWithAnInteger("/Pgun/Mode", this); 
	fParticleModeCmd->SetGuidance("Set the particle mode");
	fParticleModeCmd->SetGuidance("0 monocromatic energy");
	fParticleModeCmd->SetGuidance("1 real custom spectrum"); 
	fParticleModeCmd->SetParameterName("mode", true);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
	delete fPgunCmd;
	delete fPgunAngleCmd; 
	delete fParticleModeCmd; 
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
	if (command == fParticleModeCmd)
	{
	    G4int mode = fParticleModeCmd->GetNewIntValue(newValue); 
	    G4cout << "Command received: /Pgun/Mode " << mode << G4endl;
	    fGun->SetGunMode(mode);
	}
}

