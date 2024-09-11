#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "DetectorConstructionMessenger.hh"

namespace G4_PCM
{
    DetectorConstruction::DetectorConstruction()
        : fTargetThickness(1 * mm), // Valor predeterminado
        fMessenger(new DetectorConstructionMessenger(this)) // Crear el mensajero
    {
        DefineMaterials();
        DefineOpticalProperties();
    }

    DetectorConstruction::~DetectorConstruction()
    {
        delete fMessenger; // Eliminar el mensajero
    }

    // Define los materiales utilizados en la simulación
    void DetectorConstruction::DefineMaterials()
    {
        G4NistManager* nist = G4NistManager::Instance();

        // Definir el material realista V2O5 (óxido de vanadio)
        G4double density = 3.36 * g / cm3; // Densidad del óxido de vanadio (V2O5)
        target = new G4Material("VanadiumOxide", density, 2);
        target->AddElement(nist->FindOrBuildElement("V"), 2); // Dos átomos de vanadio
        target->AddElement(nist->FindOrBuildElement("O"), 5); // Cinco átomos de oxígeno

        // Definir el material para el detector
        E_PbWO4 = new G4Material("E_PbWO4", 8.28 * g / cm3, 3);
        E_PbWO4->AddElement(nist->FindOrBuildElement("Pb"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("W"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("O"), 4);

        // Definir el material de vacío para el mundo
        vacuum = nist->FindOrBuildMaterial("G4_Galactic");

        // Asignar propiedades ópticas al vacío
        DefineWorldOpticalProperties();
    }

    // Define las propiedades ópticas del vacío (mundo)
    void DetectorConstruction::DefineWorldOpticalProperties()
    {
        G4MaterialPropertiesTable* vacuumMPT = new G4MaterialPropertiesTable();
        G4double vacuumRindex[] = { 1.0, 1.0 }; // Índice de refracción para vacío
        G4double vacuumPhotonEnergy[] = { 0.1 * eV, 12.4 * eV };
        vacuumMPT->AddProperty("RINDEX", vacuumPhotonEnergy, vacuumRindex, 2);
        vacuum->SetMaterialPropertiesTable(vacuumMPT);
    }

    // Define las propiedades ópticas del V2O5
    void DetectorConstruction::DefineTargetOpticalProperties()
    {
        const G4int numEntries = 12;
        G4double photonEnergy[numEntries] = {
            0.496 * eV, 1.0 * eV, 2.0 * eV, 3.0 * eV,
            4.0 * eV, 5.0 * eV, 6.0 * eV, 7.0 * eV,
            8.0 * eV, 9.0 * eV, 10.0 * eV, 12.4 * eV // Energía de los fotones
        };

        // Índice de refracción para V2O5 (basado en datos experimentales)
        G4double refractiveIndexV2O5[numEntries] = {
            2.45, 2.50, 2.55, 2.60, 2.65, 2.70, 2.75, 2.80,
            2.85, 2.90, 2.95, 3.00 // Valores aproximados
        };

        // Longitud de absorción para V2O5 (absorbe principalmente UV e IR)
        G4double absorptionLengthV2O5[numEntries] = {
            0.01 * mm, 0.05 * mm, 0.1 * mm, 0.5 * mm,
            1.0 * mm, 2.0 * mm, 5.0 * mm, 10.0 * mm,
            20.0 * mm, 50.0 * mm, 100.0 * mm, 1.0 * cm
        };

        // Asignar propiedades ópticas al V2O5
        G4MaterialPropertiesTable* MPTV2O5 = new G4MaterialPropertiesTable();
        MPTV2O5->AddProperty("RINDEX", photonEnergy, refractiveIndexV2O5, numEntries);
        MPTV2O5->AddProperty("ABSLENGTH", photonEnergy, absorptionLengthV2O5, numEntries);

        target->SetMaterialPropertiesTable(MPTV2O5);
    }

    // Define las propiedades ópticas del detector (E_PbWO4)
    void DetectorConstruction::DefineDetectorOpticalProperties()
    {
        const G4int numEntries = 12;
        G4double photonEnergy[numEntries] = {
            0.496 * eV, 1.0 * eV, 2.0 * eV, 3.0 * eV,
            4.0 * eV, 5.0 * eV, 6.0 * eV, 7.0 * eV,
            8.0 * eV, 9.0 * eV, 10.0 * eV, 12.4 * eV // Energía de los fotones
        };

        G4double refractiveIndexTarget[numEntries] = {
            2.2, 2.25, 2.3, 2.35, 2.4, 2.45, 2.5, 2.55, 2.6, 2.65, 2.7, 2.75
        };

        G4double absorptionLengthTarget[numEntries] = {
            1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm,
            1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm
        };

        G4double reflectivityTarget[numEntries] = {
            0.1, 0.1, 0.1, 0.1, 0.1,
            0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
        };

        // Crear tabla de propiedades del material para el detector
        G4MaterialPropertiesTable* MPTTarget = new G4MaterialPropertiesTable();
        MPTTarget->AddProperty("RINDEX", photonEnergy, refractiveIndexTarget, numEntries);
        MPTTarget->AddProperty("ABSLENGTH", photonEnergy, absorptionLengthTarget, numEntries);
        MPTTarget->AddProperty("REFLECTIVITY", photonEnergy, reflectivityTarget, numEntries);
        E_PbWO4->SetMaterialPropertiesTable(MPTTarget);
    }

    void DetectorConstruction::DefineOpticalProperties()
    {
        DefineDetectorOpticalProperties();
        DefineTargetOpticalProperties();
    }

    G4VPhysicalVolume* DetectorConstruction::Construct()
    {
        // Definir el tamaño del mundo
        G4double worldSize = 1 * m;
        auto solidWorld = new G4Box("World", worldSize / 2, worldSize / 2, worldSize * 5);
        fWorldLog = new G4LogicalVolume(solidWorld, vacuum, "World");
        auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLog, "World", nullptr, false, 0);

        // Crear el target (óxido de vanadio)
        CreateTarget();

        // Crear el detector (plomo tungsteno)
        CreateDetector();

        // Definir propiedades ópticas y asignarlas a la superficie
        CreateOpticalSurface();

        // Definir este detector como el detector gamma
        fGammaDetector = fDetectorLog;

        return physWorld;
    }

    // Crea el target y lo posiciona en el mundo
    void DetectorConstruction::CreateTarget()
    {
        G4double innerTargetRadius = 0.0;
        G4double outerTargetRadius = 2.5 * cm;

        G4Tubs* solidTarget = new G4Tubs("Target", innerTargetRadius, outerTargetRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
        G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, target, "Target");
        G4ThreeVector targetPos = G4ThreeVector();
        new G4PVPlacement(nullptr, targetPos, logicTarget, "Target", fWorldLog, false, 0);
    }

    // Crea el detector y lo posiciona en el mundo
    void DetectorConstruction::CreateDetector()
    {
        G4double detectorSizeXY = 20 * cm;
        G4double detectorSizeZ = 10 * cm;

        auto solidDetector = new G4Box("Detector", detectorSizeXY, detectorSizeXY, detectorSizeZ);
        fDetectorLog = new G4LogicalVolume(solidDetector, E_PbWO4, "Detector");
        G4ThreeVector detectorPos = G4ThreeVector(0, 0, 20 * cm);
        new G4PVPlacement(nullptr, detectorPos, fDetectorLog, "Detector", fWorldLog, false, 0);
    }


    // Define la superficie óptica y la asigna a la geometría
    void DetectorConstruction::CreateOpticalSurface()
    {
        auto surface = new G4OpticalSurface("Surface");
        surface->SetType(dielectric_dielectric);
        surface->SetFinish(polished);
        surface->SetModel(unified);

        auto surfaceLog = new G4LogicalSkinSurface("SkinSurface", fDetectorLog, surface);
    }

    void DetectorConstruction::SetTargetThickness(G4double thickness)
    {
        fTargetThickness = thickness;
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
