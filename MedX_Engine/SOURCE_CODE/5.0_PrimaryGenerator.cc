#include "5.0_PrimaryGenerator.hh"

MyPrimaryGenerator::MyPrimaryGenerator(): fGunMode(0), fPgun(-45.0*cm), fGunAngle(0.0), energy(80*keV), fGunRadius(10.0), spectrumFileName("fSpectrum140.txt"), GeneratorMessenger(new PrimaryGeneratorMessenger(this))
{
    particleGun = new G4ParticleGun(1);
    particleTable = G4ParticleTable::GetParticleTable();
    particleName = "gamma";
    particle = particleTable -> FindParticle(particleName);
    particleGun -> SetParticleDefinition(particle);   

   if (fGunMode == 1) { InitFunction(); }
}

MyPrimaryGenerator::~MyPrimaryGenerator() {delete particleGun; delete GeneratorMessenger;}

void MyPrimaryGenerator::GeneratePrimaries(G4Event * anEvent)
{ 
    if (fGunMode == 1) // Espectro real
    {
	    realEnergy = InverseCumul(); 
	    particleGun -> SetParticleEnergy(realEnergy);
    }
	
    x0 = 2 * (G4UniformRand() - 0.5);
    y0 = 2 * (G4UniformRand() - 0.5);

    x0 = x0 * fGunRadius * cm;
    y0 = y0 * fGunRadius * cm;
    z0 = fPgun; // Messenger used
    
    x0 = x0 - 1;
    y0 = y0 - 1;

    G4ThreeVector photonPosition(x0, y0, z0);
    particleGun -> SetParticlePosition(photonPosition);

    fullAngle = true; if (fullAngle == true) {angle = 2.0;} else {angle = 1.0;}

    angleInRadians = fGunAngle * (2*pi / 360.0);
    angleInCarts = std::tan(angleInRadians);
    theta = angleInCarts * (G4UniformRand() - 0.5) * angle;
    phi   = angleInCarts * (G4UniformRand() - 0.5) * angle;
    G4ThreeVector photonMomentum(theta, phi, 1.0);
    particleGun -> SetParticleMomentumDirection(photonMomentum);

    particleGun -> GeneratePrimaryVertex(anEvent);
}

// MESSENGERS ===================================================================================================

void MyPrimaryGenerator::SetGunZpos(G4double zpos)
{
    G4cout << "Setting source position to: " << zpos << G4endl;
    if (zpos != fPgun) { fPgun = zpos; G4cout << "Source Position changed to: " << fPgun << G4endl;}
    else { G4cout << "Same Position Selected." << G4endl;}
}

void MyPrimaryGenerator::SetGunRadius(G4double radius)
{
    G4cout << "Setting source radius to: " << radius << G4endl;
    if(radius != fGunRadius) { fGunRadius = radius; G4cout << "Source radius changed to: " << fGunRadius << G4endl;}
    else { G4cout << "Same Radius selected." << G4endl; }
}

void MyPrimaryGenerator::SetGunAngle(G4double angle)
{
    G4cout << "Setting source angle to: " << angle << G4endl;
    if(angle != fGunAngle) { fGunAngle = angle; G4cout << "Source Angle changed to: " << fGunAngle << G4endl;}
    else { G4cout << "Same Angle selected." << G4endl; }
}

void MyPrimaryGenerator::SetGunMode(G4int mode)
{
    G4cout << "Setting mode to: " << mode << G4endl; 
    if(mode == 0) { fGunMode = 0; G4cout << "Monocromatic Mode" << G4endl; }
    else 
    if(mode == 1) { fGunMode = 1; G4cout << "Real Spectrum Selected" << G4endl; }
    else { G4cout << "No mode selected. Default value applied." << G4endl; }
}

// CREATE SPECTRUM, DON'T MOVE ===================================================================================

void MyPrimaryGenerator::InitFunction()
{
	// tabulated function 
	// Y is assumed positive, linear per segment, continuous
    std::vector<G4double> xx;
    std::vector<G4double> yy;
    fNPoints = 0;

	// Leer los datos desde el archivo "datos.txt"
    ReadSpectrumFromFile(spectrumFileName, xx, yy, fNPoints);

	// Mostrar los datos leídos y la cantidad de puntos
    G4cout << "Número de puntos leídos: " << fNPoints << G4endl;
    for (size_t i = 0; i < xx.size(); ++i) 
    {
        G4cout << "Energía: " << xx[i] / keV << " keV, Intensidad: " << yy[i] << G4endl; 
    }

	// copy arrays in std::vector and compute fMax
    fX.resize(fNPoints); fY.resize(fNPoints);
    fYmax = 0.;
    for (G4int j=0; j<fNPoints; j++) 
    {
        fX[j] = xx[j]; fY[j] = yy[j];
        if (fYmax < fY[j]) fYmax = fY[j];
	};

    fSlp.resize(fNPoints); //compute slopes
    for (G4int j=0; j<fNPoints-1; j++) 
    {
        fSlp[j] = (fY[j+1] - fY[j])/(fX[j+1] - fX[j]); 
    };

    fYC.resize(fNPoints); // compute cumulative function
    fYC[0] = 0.;
    for (G4int j=1; j<fNPoints; j++) 
    {
        fYC[j] = fYC[j-1] + 0.5*(fY[j] + fY[j-1])*(fX[j] - fX[j-1]);
    };     
}

G4double MyPrimaryGenerator::InverseCumul() // Function to estimate counts
{
    // tabulated function
    // Y is assumed positive, linear per segment, continuous 
    // --> cumulative function is second order polynomial
    // (see Particle Data Group: pdg.lbl.gov --> Monte Carlo techniques)
  
    G4double Yrndm = G4UniformRand() * fYC[fNPoints-1]; //choose y randomly
 
    G4int j = fNPoints - 2;  // find bin
    while ((fYC[j] > Yrndm) && (j > 0)) j--; // y_rndm --> x_rndm :  fYC(x) is second order polynomial
    
    G4double Xrndm = fX[j];
    G4double a = fSlp[j];
    
    if (a != 0.) 
    {
        G4double b = fY[j]/a, c = 2*(Yrndm - fYC[j])/a;
        G4double delta = b*b + c;
        G4int sign = 1; if (a < 0.) sign = -1;
        Xrndm += sign*std::sqrt(delta) - b;    
    } 
    else 
    if (fY[j] > 0.) 
    {
        Xrndm += (Yrndm - fYC[j])/fY[j];
    };
    
    return Xrndm;
}

void MyPrimaryGenerator::ReadSpectrumFromFile(const std::string & filename, std::vector<G4double> & xx, std::vector<G4double> & yy, G4int & fNPoints) 
{ // Function to fill the vectors
    
    std::ifstream infile(filename);
    if (!infile) 
    {
        G4cerr << "Error opening file: " << filename << G4endl;
        return;
    }
    
    G4double energy, intensity;
    fNPoints = 0; // Inicializar el número de puntos

    while (infile >> energy >> intensity) // Convertir energía de keV a las unidades internas de Geant4
    {
        xx.push_back(energy * keV);
        yy.push_back(intensity);
        fNPoints++; // Incrementar el contador de puntos
    }

    infile.close();
}