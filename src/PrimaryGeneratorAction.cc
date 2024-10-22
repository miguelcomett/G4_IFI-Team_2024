#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"


namespace G4_PCM
{

	PrimaryGeneratorAction::PrimaryGeneratorAction()
		: fPgun(-5 * cm), fGunAngle(0), // Valor predeterminado
		fMessenger(new PrimaryGeneratorMessenger(this)) // Crear el mensajero
	{
		// set up particle gun
		G4int nParticles = 1;
		fParticleGun = new G4ParticleGun(nParticles);

		// define particle properties
		const G4String& particleName = "e-";

		G4ThreeVector momentumDirection = G4ThreeVector(0, 1, 0);

		// default particle kinematic
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* particle
			= particleTable->FindParticle(particleName);
		fParticleGun->SetParticleDefinition(particle);
		fParticleGun->SetParticleMomentumDirection(momentumDirection);

	}


	void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
	{	
		// -------------------------------------------------------
		// Constantes globales
		// -------------------------------------------------------
		const G4double pi = 3.14159265358979323846; // Valor de π
		const G4double defaultRadius = 0.5 * mm;    // Radio por defecto para las fuentes

		// -------------------------------------------------------
		// Establecer el radio según el ángulo
		// -------------------------------------------------------
		G4double radius;
		if (fGunAngle == 0) {
			// Si el ángulo es 0, define un diámetro de 100 mm
			radius = 1 * cm;
		}
		else {
			// Si el ángulo no es 0, usa el diámetro por defecto
			radius = defaultRadius;
		}

		// Definir posición inicial de la partícula
		G4double x, z, y = fPgun; // Establece z en -5 cm (ajusta según sea necesario)

		// -------------------------------------------------------
		// Fuente de posición (Radial o Cuadrada)
		// -------------------------------------------------------

		// Fuente radial (descomentar para activar)
		// do {
		//     x = G4UniformRand() * (2.0 * radius) - radius;
		//     y = G4UniformRand() * (2.0 * radius) - radius;
		// } while (x * x + y * y > radius * radius);

		//Fuente cuadrada (descomentar para activar)
		x = G4UniformRand() * (2.0 * radius) - radius;
		z = G4UniformRand() * (2.0 * radius) - radius;
		/*x = 0;
		z = 0;*/
		G4ThreeVector position = G4ThreeVector(x, y, z);
	
		fParticleGun->SetParticlePosition(position);
		G4ThreeVector momentumDirection = G4ThreeVector(0.0, 1.0, 0.0); // Dirección hacia el eje Y
		fParticleGun->SetParticleMomentumDirection(momentumDirection);

		// -------------------------------------------------------
		// Dirección del haz (Cónico o Lineal)
		// -------------------------------------------------------

		// Definir el ángulo en grados y convertir a radianes
		G4double angleInDegrees = fGunAngle;
		G4double angleInRadians = angleInDegrees * (pi / 180.0);

		// Comportamiento cónico (descomentar para activar dispersión angular)
		// G4double theta, phi;
		// theta = angleInRadians * (G4UniformRand() - 0.5);
		// phi = angleInRadians * (G4UniformRand() - 0.5);
		// G4ThreeVector photonMomentum(theta, phi, 1.0);
		// fParticleGun->SetParticleMomentumDirection(photonMomentum);

		// -------------------------------------------------------
		// Escáner lineal (en eje X o Y)
		// -------------------------------------------------------

		// Escáner lineal en el eje X (por defecto activado)
	/*	x = G4UniformRand() * (2.0 * radius) - radius;
		G4ThreeVector scannerPositionX = G4ThreeVector(x, 0.0, z);
		fParticleGun->SetParticlePosition(scannerPositionX);*/

		// Escáner lineal en el eje Y (descomentar para activar)
		// G4double y_scan_range = scannerRange;
		// G4double y_position = (G4UniformRand() - 0.5) * y_scan_range;
		// G4ThreeVector scannerPositionY = G4ThreeVector(0.0, y_position, 0.0);
		// fParticleGun->SetParticlePosition(scannerPositionY);

		// -------------------------------------------------------
		// Energía de la partícula (Fija o Gaussiana)
		// -------------------------------------------------------

		// Distribución Gaussiana de energía (descomentar para activar)
		// G4double meanEnergy = 40 * keV;
		// G4double stdDev = 3.5 * keV;
		// G4double energy = G4RandGauss::shoot(meanEnergy, stdDev);

		// Energía fija (por defecto activada)
		//G4double energy = 10 * keV;
		//fParticleGun->SetParticleEnergy(energy);

		// -------------------------------------------------------
		// Generar el evento primario
		// -------------------------------------------------------
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
