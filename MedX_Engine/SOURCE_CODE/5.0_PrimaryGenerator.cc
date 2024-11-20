#include "5.0_PrimaryGenerator.hh"

PrimaryGenerator::PrimaryGenerator(DetectorConstruction * detector):GunMode(0), GunXpos(0), GunYpos(0), GunZpos(-450*mm), 
GunAngle(0.0), GunSpanX(100*mm), GunSpanY(100*mm), spectrumFile("fSpectrum140.txt"), 
GeneratorMessenger(new PrimaryGeneratorMessenger(this)), G4VUserPrimaryGeneratorAction(), fDetector(detector)
{
    particleGun = new G4ParticleGun(1);
    particleTable = G4ParticleTable::GetParticleTable();
    particleName = "gamma";
    particle = particleTable -> FindParticle(particleName);
    particleGun -> SetParticleDefinition(particle);   

   if (GunMode == 1) { SpectraFunction(); }
}

PrimaryGenerator::~PrimaryGenerator() {delete particleGun; delete GeneratorMessenger;}

void PrimaryGenerator::GeneratePrimaries(G4Event * anEvent)
{ 
    if (GunMode == 1) {realEnergy = InverseCumul(); particleGun -> SetParticleEnergy(realEnergy);}
	
    if (fDetector) {thoraxAngle = fDetector -> GetThoraxAngle();} else {thoraxAngle = 0;} // Uso del puntero fDetector para acceder a datos de DetectorConstruction

    // x0 = 2 * (G4UniformRand() - 0.5);
    // x0 = x0 * GunSpanX;
    x0 = G4RandGauss::shoot(0, GunSpanX / 1.5);
    x0 = x0 * std::cos(thoraxAngle/2);

    y0 = 2 * (G4UniformRand() - 0.5);
    y0 = y0 * GunSpanY;

    x0 = x0 + GunXpos; 
    y0 = y0 + GunYpos;
    z0 = GunZpos; 

    photonPosition = G4ThreeVector(x0, y0, z0);
    particleGun -> SetParticlePosition(photonPosition);

    fullAngle = true; if (fullAngle == true) {angle = 2.0;} else {angle = 1.0;}

    angleInRadians = GunAngle * (2*pi / 360.0);
    angleInCarts = std::tan(angleInRadians);
    
    theta = angleInCarts * (G4UniformRand() - 0.5) * angle;
    phi   = angleInCarts * (G4UniformRand() - 0.5) * angle;
    
    photonMomentum = G4ThreeVector(theta, phi, 1.0);
    particleGun -> SetParticleMomentumDirection(photonMomentum);

    particleGun -> GeneratePrimaryVertex(anEvent);
}

// Messenger ==============================================================================================================================

void PrimaryGenerator::SetGunXpos(G4double GunXpos)
{
    G4cout << "Setting source position to: " << GunXpos << G4endl;
    if (this -> GunXpos != GunXpos) {this -> GunXpos = GunXpos; G4cout << "Source Position changed to: " << GunXpos << G4endl;}
    else { G4cout << "Same Position Selected." << G4endl;}
}

void PrimaryGenerator::SetGunYpos(G4double GunYpos)
{
    G4cout << "Setting source position to: " << GunYpos << G4endl;
    if (this -> GunYpos != GunYpos) {this -> GunYpos = GunYpos; G4cout << "Source Position changed to: " << GunYpos << G4endl;}
    else { G4cout << "Same Position Selected." << G4endl;}
}

void PrimaryGenerator::SetGunZpos(G4double GunZpos)
{
    G4cout << "Setting source position to: " << GunZpos << G4endl;
    if (this -> GunZpos != GunZpos) {this -> GunZpos = GunZpos; G4cout << "Source Position changed to: " << GunZpos << G4endl;}
    else { G4cout << "Same Position Selected." << G4endl;}
}

void PrimaryGenerator::SetGunSpanX(G4double SpanX)
{
    G4cout << "Setting source Span to: " << SpanX << G4endl;
    if(SpanX != GunSpanX) { GunSpanX = SpanX; G4cout << "Source Span changed to: " << GunSpanX << G4endl;}
    else { G4cout << "Same Span selected." << G4endl; }
}

void PrimaryGenerator::SetGunSpanY(G4double SpanY)
{
    G4cout << "Setting source Span to: " << SpanY << G4endl;
    if(SpanY != GunSpanY) { GunSpanY = SpanY; G4cout << "Source Span changed to: " << GunSpanY << G4endl;}
    else { G4cout << "Same Span selected." << G4endl; }
}

void PrimaryGenerator::SetGunAngle(G4double angle)
{
    G4cout << "Setting source angle to: " << angle << G4endl;
    if(angle != GunAngle) { GunAngle = angle; G4cout << "Source Angle changed to: " << GunAngle << G4endl;}
    else { G4cout << "Same Angle selected." << G4endl; }
}

void PrimaryGenerator::SetGunMode(G4int mode)
{
    G4cout << "Setting mode to: " << mode << G4endl; 
    if(mode == 0) { GunMode = 0; G4cout << "Monocromatic Mode" << G4endl; }
    else 
    if(mode == 1) { GunMode = 1; G4cout << "Real Spectrum Selected" << G4endl; }
    else { G4cout << "No mode selected. Default value applied." << G4endl; }
}

// Create Ratiation Spectra ====================================================================================================================

void PrimaryGenerator::SpectraFunction() // tabulated function // Y is assumed positive, linear per segment, continuous
{
    std::vector<G4double> xx;
    std::vector<G4double> yy;
    fNPoints = 0;

    ReadSpectrumFromFile(spectrumFile, xx, yy, fNPoints);

    G4cout << "Número de puntos leídos: " << fNPoints << G4endl;
    for (size_t i = 0; i < xx.size(); ++i) {G4cout << "Energía: " << xx[i] / keV << " keV, Intensidad: " << yy[i] << G4endl;}

	// copy arrays in std::vector and compute fMax
    fX.resize(fNPoints); fY.resize(fNPoints);
    fYmax = 0.0;
    for (G4int j=0; j<fNPoints; j++) {fX[j] = xx[j]; fY[j] = yy[j]; if (fYmax < fY[j]) fYmax = fY[j];};

    fSlp.resize(fNPoints); //compute slopes
    for (G4int j=0; j<fNPoints-1; j++) {fSlp[j] = (fY[j+1] - fY[j])/(fX[j+1] - fX[j]);};

    fYC.resize(fNPoints); // compute cumulative function
    fYC[0] = 0.;
    for (G4int j=1; j<fNPoints; j++) {fYC[j] = fYC[j-1] + 0.5*(fY[j] + fY[j-1])*(fX[j] - fX[j-1]);};     
}

G4double PrimaryGenerator::InverseCumul() // Function to estimate counts // --> cumulative function is second order polynomial // (see Particle Data Group: pdg.lbl.gov --> Monte Carlo techniques)
{ 
    G4double Yrndm = G4UniformRand() * fYC[fNPoints-1]; //choose y randomly
 
    G4int j = fNPoints - 2;  // find bin
    while ((fYC[j] > Yrndm) && (j > 0)) j--; // y_rndm --> x_rndm :  fYC(x) is second order polynomial
    
    G4double Xrndm = fX[j];
    G4double a = fSlp[j];
    
    if (a != 0.0) 
    {
        G4double b = fY[j]/a, c = 2*(Yrndm - fYC[j])/a;
        G4double delta = b*b + c;
        G4int sign = 1; if (a < 0.) sign = -1;
        Xrndm += sign*std::sqrt(delta) - b;    
    } 
    else if (fY[j] > 0.0) {Xrndm += (Yrndm - fYC[j])/fY[j];};
    
    return Xrndm;
}

void PrimaryGenerator::ReadSpectrumFromFile(const std::string & filename, std::vector<G4double> & xx, std::vector<G4double> & yy, G4int & fNPoints) // Function to fill the vectors
{ 
    std::ifstream infile(filename);
    if (!infile) {G4cerr << "Error opening file: " << filename << G4endl; return;}
    
    G4double energy, intensity;
    fNPoints = 0; 

    while (infile >> energy >> intensity) // Convertir energía de keV a las unidades internas de Geant4
    {
        xx.push_back(energy * keV);
        yy.push_back(intensity);
        fNPoints++; 
    }

    infile.close();
}