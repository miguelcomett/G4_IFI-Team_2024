#ifndef G4_PCM_DETECTOR_CONSTRUCTION_H
#define G4_PCM_DETECTOR_CONSTRUCTION_H 1

#include "G4NistManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "DetectorConstructionMessenger.hh"
#include "G4LogicalBorderSurface.hh"

namespace G4_PCM
{
    class DetectorConstructionMessenger; // Forward declaration

    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        DetectorConstruction();
        ~DetectorConstruction() override;

        G4VPhysicalVolume* Construct() override;

        void SetTargetThickness(G4double thickness);

        G4LogicalVolume* GetOpticalPhotonDetector() const { return fGammaDetector; }

    private:
        G4LogicalVolume* fGammaDetector = nullptr;
        G4LogicalVolume* fWorldLog = nullptr;  // Añadido
        G4LogicalVolume* fDetectorLog = nullptr; // Añadido
        G4LogicalVolume* fTargetLog = nullptr; // Añadido
        G4double fTargetThickness = 60 * mm; // Valor predeterminado

        G4UIcmdWithADoubleAndUnit* fTargetThicknessCmd;
        DetectorConstructionMessenger* fMessenger; // Pointer to the messenger

        G4Material* target; // Corregido
        G4Material* vacuum; // Corregido
        G4Material* E_PbWO4; // Corregido
        G4Material* detector; // Corregido

        void DefineMaterials();
        void DefineOpticalProperties();

        // Añadido: Métodos privados
        void CreateTarget();
        void CreateDetector();
        void CreateOpticalSurface();
        void DefineWorldOpticalProperties();
        void DefineDetectorOpticalProperties();
        void DefineTargetOpticalProperties();
    };
}

#endif // G4_PCM_DETECTOR_CONSTRUCTION_H
