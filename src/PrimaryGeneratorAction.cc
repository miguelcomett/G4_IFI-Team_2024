#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"


namespace G4_PCM
{

	PrimaryGeneratorAction::PrimaryGeneratorAction()
		: fPgun(-50 * cm), fGunAngle(20), // Valor predeterminado
		fMessenger(new PrimaryGeneratorMessenger(this)) // Crear el mensajero
	{
		// set up particle gun
		G4int nParticles = 1;
		fParticleGun = new G4ParticleGun(nParticles);

		// define particle properties
		const G4String& particleName = "gamma";

		G4ThreeVector momentumDirection = G4ThreeVector(0, 0, 1);

		// default particle kinematic
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* particle
			= particleTable->FindParticle(particleName);
		fParticleGun->SetParticleDefinition(particle);
		fParticleGun->SetParticleMomentumDirection(momentumDirection);

	}


	void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
	{	
		if(fGunAngle == 0)
		{	
			// Randomize x and y starting point within a 1 mm diameter
			radius = 50 * mm; // hay que cambiarlo .5*mm por defecto
		}
		else
		{
			// Randomize x and y starting point within a 1 mm diameter
			radius = 0.5 * mm; // hay que cambiarlo .5*mm por defecto
		
		}
			
		// generate random x and y positions within that radius
		double x, y, z = fPgun; // Aquí estableces z en -5 cm

		// to avoid using slow methods like sin and cos,
		// we generate random values in a cube and regect the ones
		// outside of a circle. This way 
		do {
			x = G4UniformRand() * (2.0 * radius) - radius;
			y = G4UniformRand() * (2.0 * radius) - radius;
		} while (x * x + y * y > radius * radius);

		// Define el valor de π
		const G4double pi = 3.14159265358979323846;

		// Define el ángulo en grados
		G4double angleInDegrees = fGunAngle;  // 45 grados

		// Convierte el ángulo a radianes
		G4double angleInRadians = angleInDegrees * (pi / 180.0);

		// Add conic behaviour
		G4double theta, phi;
		theta = angleInRadians * (G4UniformRand() - 0.5);
		phi = angleInRadians * (G4UniformRand() - 0.5);
		G4ThreeVector photonMomentum(theta, phi, 1.0);
		fParticleGun->SetParticleMomentumDirection(photonMomentum);

		G4ThreeVector position = G4ThreeVector(x, y, z);
		// G4ThreeVector position = G4ThreeVector(x, y, z);
		fParticleGun->SetParticlePosition(position);

		// randomize energy with a .127 MeV std:dev gaussean distribution for an e-
		// Here, chance for a photon of 15 MeV with 4 stdDev 👍
		// G4double meanEnergy = 6. * MeV;
		// G4double stdDev = .127 * MeV;
		// G4double energy = G4RandGauss::shoot(meanEnergy, stdDev);
		G4double energy = 0.01 * keV;

		// fParticleGun->SetParticleEnergy(energy);


		// satisfy "generate primaries" here.
		fParticleGun->GeneratePrimaryVertex(event);
	}

	PrimaryGeneratorAction::~PrimaryGeneratorAction() {
		delete fParticleGun;
		delete fMessenger; // Eliminar el mensajero
	}

	void PrimaryGeneratorAction::SetGunZpos(G4double zpos)
	{
		G4cout << "Setting source position to: " << zpos << G4endl;
		if (zpos != fPgun) {
			fPgun = zpos;
			G4cout << "Source Position changed to: " << fPgun << G4endl;

			// Forzar la actualización de la geometría
			//G4RunManager::GetRunManager()->ReinitializeGeometry();
		}
		else {
			G4cout << "Source Position Changed." << G4endl;
		}
	}
	
	void PrimaryGeneratorAction::SetGunAngle(G4double angle)
	{
		G4cout << "Setting source angle to: " << angle << G4endl;
		if(angle != fGunAngle)
		{
			fGunAngle = angle; 
			G4cout << "Source Angle changed to: " << fGunAngle << G4endl;
		}
		else {
			G4cout << "Source Angle Changed." << G4endl;
		}
	}
}
