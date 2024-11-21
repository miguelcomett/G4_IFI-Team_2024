#include "5.0_PrimaryGenerator.hh"

PrimaryGenerator::PrimaryGenerator(DetectorConstruction * detector) : SpectraMode(0), Xpos(300.0*mm), Ypos(0.0*mm), Zpos(-450*mm), 
GunAngle(0.0), SpanX(10*mm), SpanY(10*mm), spectrumFile("fSpectrum140.txt"), 
GeneratorMessenger(new PrimaryGeneratorMessenger(this)), G4VUserPrimaryGeneratorAction(), fDetector(detector)
{
    particleGun = new G4ParticleGun(1);
    particleTable = G4ParticleTable::GetParticleTable();
    particleName = "gamma";
    particle = particleTable -> FindParticle(particleName);
    particleGun -> SetParticleDefinition(particle);   

    if (SpectraMode == 1) { SpectraFunction(); }
}

PrimaryGenerator::~PrimaryGenerator() {delete particleGun; delete GeneratorMessenger;}

void PrimaryGenerator::GeneratePrimaries(G4Event * anEvent)
{ 
    if (SpectraMode == 1) {RealEnergy = InverseCumul(); particleGun -> SetParticleEnergy(RealEnergy);}
	
    if (fDetector) {thoraxAngle = fDetector -> GetThoraxAngle();} else {thoraxAngle = 0;}

    // x0 = 2 * (G4UniformRand() - 0.5);
    // x0 = x0 * SpanX;
    x0 = G4RandGauss::shoot(0, fPgunSpanX / 1.5);
    x0 = x0 * std::cos(thoraxAngle/2);

    y0 = 2 * (G4UniformRand() - 0.5);
    y0 = y0 * SpanY;

    x0 = x0 + Xpos; 
    y0 = y0 + Ypos;
    z0 = Zpos; 

    photonPosition = G4ThreeVector(x0, y0, z0);
    particleGun -> SetParticlePosition(photonPosition);

    AngleInCarts = std::tan(GunAngle * (2*pi / 360.0));
    theta = AngleInCarts * (G4UniformRand() - 0.5) * 2;
    phi   = AngleInCarts * (G4UniformRand() - 0.5) * 2;
    
    photonMomentum = G4ThreeVector(theta, phi, 1.0);
    particleGun -> SetParticleMomentumDirection(photonMomentum);

    particleGun -> GeneratePrimaryVertex(anEvent);
}

// Messengers ==============================================================================================================================

void PrimaryGenerator::SetGunXpos(G4double newXpos)
{if (newXpos != Xpos) {Xpos = newXpos; G4cout << "Source Position changed to: " << Xpos << G4endl;} else {G4cout << "Same Position Selected." << G4endl;}}

void PrimaryGenerator::SetGunYpos(G4double newYpos)
{if (newYpos != Ypos) {Ypos = newYpos; G4cout << "Source Position changed to: " << Ypos << G4endl;} else { G4cout << "Same Position Selected." << G4endl;}}

void PrimaryGenerator::SetGunZpos(G4double newZpos)
{if (newZpos != Zpos) {Zpos = newZpos; G4cout << "Source Position changed to: " << Zpos << G4endl;} else {G4cout << "Same Position Selected." << G4endl;}}

void PrimaryGenerator::SetGunSpanX(G4double newSpanX)
{if(newSpanX != SpanX) {SpanX = newSpanX; G4cout << "Source Span changed to: " << SpanX << G4endl;} else {G4cout << "Same Span selected." << G4endl;}}

void PrimaryGenerator::SetGunSpanY(G4double newSpanY)
{if(newSpanY != SpanY) {SpanY = newSpanY; G4cout << "Source Span changed to: " << SpanY << G4endl;} else {G4cout << "Same Span selected." << G4endl;}}

void PrimaryGenerator::SetGunAngle(G4double newAngle)
{if(newAngle != GunAngle) {GunAngle = newAngle; G4cout << "Source Angle changed to: " << GunAngle << G4endl;} else {G4cout << "Same Angle selected." << G4endl;}}

void PrimaryGenerator::SetGunMode(G4int newMode)
{
    if(newMode == 0) {SpectraMode = 0; G4cout << "Monocromatic Mode Selected" << G4endl;}
    if(newMode == 1) {SpectraMode = 1; G4cout << "Real Spectrum Selected" << G4endl;}
    else {G4cout << "No mode selected. Default value applied." << G4endl;}
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
