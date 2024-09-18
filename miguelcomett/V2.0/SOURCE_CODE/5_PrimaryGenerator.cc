#include "5_PrimaryGenerator.hh"


MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1);

    G4ParticleTable * particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "gamma";
    G4ParticleDefinition * particle = particleTable -> FindParticle(particleName);

    fParticleGun -> SetParticleDefinition(particle);
    fParticleGun -> SetParticleEnergy(20 * keV);
}


MyPrimaryGenerator::~MyPrimaryGenerator() {delete fParticleGun;}


void MyPrimaryGenerator::GeneratePrimaries(G4Event * anEvent)
{ 
    G4double x0, y0, z0;
    G4double radius = 10.0;

    x0 = 2 * (G4UniformRand() - 0.5);
    y0 = 2 * (G4UniformRand() - 0.5);
    // y0 = y0 * std::sqrt(1 - std::pow(x0, 2));

    x0 = x0 * radius * cm;
    y0 = y0 * radius * cm;
    z0 = - 50.0 * cm;

    G4ThreeVector photonPosition(x0, y0, z0);
    fParticleGun -> SetParticlePosition(photonPosition);

    const G4double pi = 3.14159265358979323846;
    G4double theta, phi, angle, angleInDegrees, angleInRadians, angleInCarts;

    angleInDegrees = 0.0;

    G4bool fullAngle = true;
    if (fullAngle == true) {angle = 2.0;}
    else {angle = 1.0;}

    angleInRadians = angleInDegrees * (2*pi / 360.0);
    angleInCarts = std::tan(angleInRadians);

    theta = angleInCarts * (G4UniformRand() - 0.5) * angle;
    phi   = angleInCarts * (G4UniformRand() - 0.5) * angle;
    G4ThreeVector photonMomentum(theta, phi, 1.0);
    fParticleGun -> SetParticleMomentumDirection(photonMomentum);

    fParticleGun -> GeneratePrimaryVertex(anEvent);
}