#include "5.1_GeneratorMessenger.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(MyPrimaryGenerator * gun) : fGun(gun)
{
	fPgunX = new G4UIcmdWithADoubleAndUnit("/Pgun/X", this);
	fPgunX -> SetGuidance("Set the source X position.");
	fPgunX -> SetParameterName("Xpos", true);

	fPgunY = new G4UIcmdWithADoubleAndUnit("/Pgun/Y", this);
	fPgunY -> SetGuidance("Set the source Y position.");
	fPgunY -> SetParameterName("Ypos", true);
	
	fPgunZ = new G4UIcmdWithADoubleAndUnit("/Pgun/Z", this);
	fPgunZ -> SetGuidance("Set the source Z position.");
	fPgunZ -> SetParameterName("Zpos", true);

	fPgunSpanX = new G4UIcmdWithADouble("/Pgun/SpanX", this);
	fPgunSpanX -> SetGuidance("Set the source X length.");
	fPgunSpanX -> SetParameterName("SpanY", true);

	fPgunSpanY = new G4UIcmdWithADouble("/Pgun/SpanY", this);
	fPgunSpanY -> SetGuidance("Set the source Y length.");
	fPgunSpanY -> SetParameterName("SpanY", true);

	fPgunAngle = new G4UIcmdWithADouble("/Pgun/Angle", this);
	fPgunAngle -> SetGuidance("Set the source angle.");
	fPgunAngle -> SetParameterName("angle", true);
	//fPgunAngle->SetDefaultUnit("deg");

	fParticleMode = new G4UIcmdWithAnInteger("/Pgun/Mode", this); 
	fParticleMode -> SetGuidance("Set the particle mode");
	fParticleMode -> SetGuidance("0 monocromatic energy");
	fParticleMode -> SetGuidance("1 real custom spectrum"); 
	fParticleMode -> SetParameterName("mode", true);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
	delete fPgunX; delete fPgunY; delete fPgunZ;
	delete fPgunSpanX; delete fPgunSpanY;
	delete fPgunAngle; 
	delete fParticleMode; 
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command, G4String newValue)
{
	if (command == fPgunX)
	{
	    G4double Xpos = fPgunX -> GetNewDoubleValue(newValue);
	    G4cout << "Command received: /Pgun/X " << Xpos << G4endl;
	    fGun -> SetGunXpos(Xpos);
	}

	if (command == fPgunY)
	{
	    G4double Ypos = fPgunY -> GetNewDoubleValue(newValue);
	    G4cout << "Command received: /Pgun/Y " << Ypos << G4endl;
	    fGun -> SetGunYpos(Ypos);
	}

	if (command == fPgunZ)
	{
	    G4double Zpos = fPgunZ -> GetNewDoubleValue(newValue);
	    G4cout << "Command received: /Pgun/Z " << Zpos << G4endl;
	    fGun -> SetGunZpos(Zpos);
	}

	if (command == fPgunSpanX)
	{
		G4double SpanX = fPgunSpanX -> GetNewDoubleValue(newValue);
	    G4cout << "Command received: /Pgun/SpanX " << SpanX << G4endl;
	    fGun -> SetGunSpanX(SpanX);
	}

	if (command == fPgunSpanY)
	{
		G4double SpanY = fPgunSpanX -> GetNewDoubleValue(newValue);
	    G4cout << "Command received: /Pgun/SpanY " << SpanY << G4endl;
	    fGun -> SetGunSpanY(SpanY);
	}
	
	if (command == fPgunAngle)
	{
	    G4double angle = fPgunAngle -> GetNewDoubleValue(newValue);
	    G4cout << "Command received: /Pgun/Angle " << angle << G4endl;
	    fGun -> SetGunAngle(angle);
	}

	if (command == fParticleMode)
	{
	    G4int mode = fParticleMode -> GetNewIntValue(newValue); 
	    G4cout << "Command received: /Pgun/Mode " << mode << G4endl;
	    fGun -> SetGunMode(mode);
	}
}