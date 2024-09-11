#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"

namespace G4_PCM
{
    PrimaryGeneratorAction::PrimaryGeneratorAction()
    {
        // Crear una instancia de G4GeneralParticleSource
        fParticleSource = new G4GeneralParticleSource();

        // Configuración inicial de la fuente de partículas
        fParticleSource->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton"));

        //// Configura la dirección de los fotones ópticos
        //G4UImanager* UI = G4UImanager::GetUIpointer();
        //UI->ApplyCommand("/gps/direction 0 0 1)");
    }

    PrimaryGeneratorAction::~PrimaryGeneratorAction()
    {
        delete fParticleSource;
    }

    void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
    {
        // Genera el vértice primario
        fParticleSource->GeneratePrimaryVertex(event);
    }
}
