#ifndef PRIMARY_GENERATOR_MESSENGER_H
#define PRIMARY_GENERATOR_MESSENGER_H

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"

#include "5.0_PrimaryGenerator.hh"

class MyPrimaryGenerator; 
class PrimaryGeneratorMessenger : public G4UImessenger
{
	public:

		PrimaryGeneratorMessenger(MyPrimaryGenerator * gun);
		~PrimaryGeneratorMessenger() override;
		void SetNewValue(G4UIcommand * command, G4String newValue) override;
    	
	private:
		
		MyPrimaryGenerator * fGun;
		
		G4UIcmdWithADoubleAndUnit * fPgunCmd; 
		G4UIcmdWithADouble * fPgunAngleCmd;
		G4UIcmdWithAnInteger * fParticleModeCmd; 
    
};

#endif
