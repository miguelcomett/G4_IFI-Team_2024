#ifndef PrimaryGenerator_hh
#define PrimaryGenerator_hh

#include <iomanip>
#include "Randomize.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"

#include "3.0_DetectorConstruction.hh"
#include "6.1_Run.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
    public:
        MyPrimaryGenerator();
        ~ MyPrimaryGenerator();

        virtual void GeneratePrimaries(G4Event *);

        G4ParticleGun * GetParticleGun() const {return ParticleGun;}
    
    private:
        G4ParticleGun * ParticleGun;        
};

#endif