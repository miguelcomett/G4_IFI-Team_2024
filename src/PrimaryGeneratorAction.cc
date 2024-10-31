#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

#include "Randomize.hh"


namespace G4_PCM
{

	PrimaryGeneratorAction::PrimaryGeneratorAction()
		: fPgun(-50 * cm), fGunAngle(20), fGunMode(0), 
		energy(80 * keV),// Valor predeterminado
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
		
		// Function to realSpectrum
		//InitFunction(); 

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
			radius = 15 * cm;
		}
		else {
			// Si el ángulo no es 0, usa el diámetro por defecto
			radius = defaultRadius;
		}

		// Definir posición inicial de la partícula
		G4double x, y, z = fPgun; // Establece z en -5 cm (ajusta según sea necesario)

		// -------------------------------------------------------
		// Fuente de posición (Radial o Cuadrada)
		// -------------------------------------------------------

		// Fuente radial (descomentar para activar)
		// do {
		//     x = G4UniformRand() * (2.0 * radius) - radius;
		//     y = G4UniformRand() * (2.0 * radius) - radius;
		// } while (x * x + y * y > radius * radius);

		//Fuente cuadrada (descomentar para activar)
		G4double meanDist = 0.5;
		G4double stdDev = 0.5 * keV;
		G4double Dist = G4RandGauss::shoot(meanDist, stdDev);
	/*	x = (Dist * (2.0 * radius) - radius);
		y = ((Dist * (2.0 * radius) - radius)-0);*/
		x = G4UniformRand() * (2.0 * radius) - radius;
		y = ((G4UniformRand() * (2.0 * radius) - radius)+10);
		G4ThreeVector position = G4ThreeVector(x, y, z);
	
		fParticleGun->SetParticlePosition(position);

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
		//Energia mode
		if (fGunMode == 0)
		{	 //Energia monocromatica
			fParticleGun->SetParticleEnergy(energy);
		}
		
		else if (fGunMode == 1)
		{
			G4double realEnergy = InverseCumul(); //Espectro real
			fParticleGun->SetParticleEnergy(realEnergy);
		}
		else
		{
			fParticleGun->SetParticleEnergy(energy);
			G4cout << "No valid mode. Standard value applied." << G4endl; 
		}

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
	
	void PrimaryGeneratorAction::SetGunMode(G4int mode)
	{
		G4cout << "Setting mode to: " << mode << G4endl; 
		if(mode == 0)
		{
			fGunMode = 0; 
			G4cout << "Monocromatic Mode" << G4endl ;
		}
		else if(mode == 1)
		{
			fGunMode = 1; 
			G4cout << "Real Spectrum Selected" << G4endl; 
		}
		else
		{
			G4cout << "No mode selected. Default value applied." << G4endl; 
		}
		
	
	}
	
	void PrimaryGeneratorAction::InitFunction()
	{
		// tabulated function 
		// Y is assumed positive, linear per segment, continuous
		//
		std::vector<G4double> xx;
		std::vector<G4double> yy;
		fNPoints = 0;

		// Leer los datos desde el archivo "datos.txt"
		ReadSpectrumFromFile("fSpectrum.txt", xx, yy, fNPoints);

		// Mostrar los datos leídos y la cantidad de puntos
		G4cout << "Número de puntos leídos: " << fNPoints << G4endl;
		for (size_t i = 0; i < xx.size(); ++i) {
			G4cout << "Energía: " << xx[i] / keV << " keV, Intensidad: " << yy[i] << G4endl;
		}


		//copy arrays in std::vector and compute fMax
		//
		fX.resize(fNPoints); fY.resize(fNPoints);
		fYmax = 0.;
		for (G4int j=0; j<fNPoints; j++) {
			fX[j] = xx[j]; fY[j] = yy[j];
			if (fYmax < fY[j]) fYmax = fY[j];
		};

		//compute slopes
		//
		fSlp.resize(fNPoints);
		for (G4int j=0; j<fNPoints-1; j++) { 
			fSlp[j] = (fY[j+1] - fY[j])/(fX[j+1] - fX[j]);
		};

		//compute cumulative function
		//
		fYC.resize(fNPoints);  
		fYC[0] = 0.;
		for (G4int j=1; j<fNPoints; j++) {
			fYC[j] = fYC[j-1] + 0.5*(fY[j] + fY[j-1])*(fX[j] - fX[j-1]);
		};     
	}
	//Fcuntion to estimate counts
	G4double PrimaryGeneratorAction::InverseCumul()
	{
	  // tabulated function
	  // Y is assumed positive, linear per segment, continuous 
	  // --> cumulative function is second order polynomial
	  // (see Particle Data Group: pdg.lbl.gov --> Monte Carlo techniques)
	  
	  //choose y randomly
	  G4double Yrndm = G4UniformRand()*fYC[fNPoints-1];
	  //find bin
	  G4int j = fNPoints-2;
	  while ((fYC[j] > Yrndm) && (j > 0)) j--;
	  //y_rndm --> x_rndm :  fYC(x) is second order polynomial
	  G4double Xrndm = fX[j];
	  G4double a = fSlp[j];
	  if (a != 0.) {
	    G4double b = fY[j]/a, c = 2*(Yrndm - fYC[j])/a;
	    G4double delta = b*b + c;
	    G4int sign = 1; if (a < 0.) sign = -1;
	    Xrndm += sign*std::sqrt(delta) - b;    
	  } else if (fY[j] > 0.) {
	    Xrndm += (Yrndm - fYC[j])/fY[j];
	  };
	  return Xrndm;
	}
	//Function to fill the vectors
	void PrimaryGeneratorAction::ReadSpectrumFromFile(const std::string& filename, std::vector<G4double>& xx, std::vector<G4double>& yy, G4int& fNPoints) {
        	std::ifstream infile(filename);
		if (!infile) {
        		G4cerr << "Error opening file: " << filename << G4endl;
        		return;
    		}

    		G4double energy, intensity;
    		fNPoints = 0; // Inicializar el número de puntos

    		while (infile >> energy >> intensity) {
        // Convertir energía de keV a las unidades internas de Geant4
        		xx.push_back(energy * keV);
        		yy.push_back(intensity);
        		fNPoints++; // Incrementar el contador de puntos
    		}

    		infile.close();
		}
	
}
