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
        void SetMaterial(G4Material* material); // Añadido

        G4LogicalVolume* GetOpticalPhotonDetector() const { return fGammaDetector; }

    private:
        G4Material* DefineRealisticMaterial();
        G4LogicalVolume* fGammaDetector = nullptr;
        G4LogicalVolume* fWorldLog = nullptr;  // Añadido
        G4LogicalVolume* fDetectorLog = nullptr; // Añadido
        G4LogicalVolume* fTargetLog = nullptr; // Añadido
        G4double fTargetThickness = 60 * mm; // Valor predeterminado

        G4UIcmdWithADoubleAndUnit* fTargetThicknessCmd;
        DetectorConstructionMessenger* fMessenger; // Pointer to the messenger

        G4Material* target, * vacuum, * E_PbWO4, * detector; //*E_V2O5

        void DefineMaterials();
        void DefineOpticalProperties();
    };
}

#endif // G4_PCM_DETECTOR_CONSTRUCTION_H
