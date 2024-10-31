#include "5_PrimaryGenerator.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
    ParticleGun = new G4ParticleGun(1);

    G4ParticleTable * particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "gamma";
    G4ParticleDefinition * particle = particleTable -> FindParticle(particleName);

    ParticleGun -> SetParticleDefinition(particle);
    ParticleGun -> SetParticleEnergy(40 * keV);
}

MyPrimaryGenerator::~MyPrimaryGenerator() {delete ParticleGun;}

void MyPrimaryGenerator::GeneratePrimaries(G4Event * anEvent)
{ 
    G4double x0, y0, z0;
    G4double radius = 40.0;

    x0 = 2 * (G4UniformRand() - 0.5);
    y0 = 2 * (G4UniformRand() - 0.5);
    // y0 = y0 * std::sqrt(1 - std::pow(x0, 2));

    x0 = x0 * radius * cm;
    y0 = y0 * radius * cm;
    z0 = - 50.0 * cm;

    G4ThreeVector photonPosition(x0, y0, z0);
    ParticleGun -> SetParticlePosition(photonPosition);

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
    ParticleGun -> SetParticleMomentumDirection(photonMomentum);

    ParticleGun -> GeneratePrimaryVertex(anEvent);
}