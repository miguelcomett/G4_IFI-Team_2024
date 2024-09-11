#ifndef G4_PCM_PRIMARY_GENERATOR_ACTION_H
#define G4_PCM_PRIMARY_GENERATOR_ACTION_H 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"

namespace G4_PCM
{
    class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
    {
    public:
        PrimaryGeneratorAction();
        ~PrimaryGeneratorAction();

        virtual void GeneratePrimaries(G4Event*);

    private:
        G4GeneralParticleSource* fParticleSource; // Fuente de partículas generalizada
    };
}

#endif
