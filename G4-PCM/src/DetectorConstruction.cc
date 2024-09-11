#include "DetectorConstruction.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4RunManager.hh"

namespace G4_PCM
{
    DetectorConstruction::DetectorConstruction()
        : fTargetThickness(1 * mm), // Valor predeterminado
        fMessenger(new DetectorConstructionMessenger(this)) // Crear el mensajero
    {
        DefineMaterials();
    }

    DetectorConstruction::~DetectorConstruction()
    {
        delete fMessenger; // Eliminar el mensajero
    }

    G4Material* DetectorConstruction::DefineRealisticMaterial()
    {
        //G4double density = 2.2 * g / cm3; // Densidad del vidrio de sílice
        //G4Material* realisticMaterial = new G4Material("SilicaGlass", density, 2);

        //G4NistManager* nist = G4NistManager::Instance();
        //realisticMaterial->AddElement(nist->FindOrBuildElement("Si"), 1);
        //realisticMaterial->AddElement(nist->FindOrBuildElement("O"), 2);

        G4double density = 3.36 * g / cm3; // Densidad del óxido de vanadio (V2O5)
        G4Material* realisticMaterial = new G4Material("VanadiumOxide", density, 2);

        G4NistManager* nist = G4NistManager::Instance();
        realisticMaterial->AddElement(nist->FindOrBuildElement("V"), 2); // Dos átomos de vanadio
        realisticMaterial->AddElement(nist->FindOrBuildElement("O"), 5); // Cinco átomos de oxígeno


        return realisticMaterial;
    }

    void DetectorConstruction::DefineMaterials()
    {
        G4NistManager* nist = G4NistManager::Instance();

        // Definir vidrio de sílice (target)
        target = DefineRealisticMaterial();

        // Material para el detector
        E_PbWO4 = new G4Material("E_PbWO4", 8.28 * g / cm3, 3);
        E_PbWO4->AddElement(nist->FindOrBuildElement("Pb"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("W"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("O"), 4);

        // Material de vacío para el mundo
        vacuum = nist->FindOrBuildMaterial("G4_Galactic");

        // Asignar propiedades ópticas al vacío
        G4MaterialPropertiesTable* vacuumMPT = new G4MaterialPropertiesTable();
        G4double vacuumRindex[] = { 1.0, 1.0 }; // Índice de refracción para vacío
        G4double vacuumPhotonEnergy[] = { 0.1 * eV, 12.4 * eV };
        vacuumMPT->AddProperty("RINDEX", vacuumPhotonEnergy, vacuumRindex, 2);
        vacuum->SetMaterialPropertiesTable(vacuumMPT);
    }

    void DetectorConstruction::DefineOpticalProperties()
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



        // Propiedades ópticas para el detector (E_PbWO4)
        G4double refractiveIndexDetector[numEntries] = {
            2.2, 2.25, 2.3, 2.35, 2.4, 2.45, 2.5, 2.55, 2.6, 2.65, 2.7, 2.75
        };

        G4double absorptionLengthDetector[numEntries] = {
            1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm,
            1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm, 1 * cm
        };

        G4double reflectivityDetector[numEntries] = {
            0.1, 0.1, 0.1, 0.1, 0.1,
            0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1
        };

        // Crear tabla de propiedades del material para el detector
        G4MaterialPropertiesTable* MPTDetector = new G4MaterialPropertiesTable();
        MPTDetector->AddProperty("RINDEX", photonEnergy, refractiveIndexDetector, numEntries);
        MPTDetector->AddProperty("ABSLENGTH", photonEnergy, absorptionLengthDetector, numEntries);
        MPTDetector->AddProperty("REFLECTIVITY", photonEnergy, reflectivityDetector, numEntries);
        E_PbWO4->SetMaterialPropertiesTable(MPTDetector);
    }

    G4VPhysicalVolume* DetectorConstruction::Construct()
    {
        // Definir el tamaño del mundo
        G4double worldSize = 1 * m;
        auto solidWorld = new G4Box("World", worldSize / 2, worldSize / 2, worldSize * 5);
        fWorldLog = new G4LogicalVolume(solidWorld, vacuum, "World");
        auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLog, "World", nullptr, false, 0);

        // Crear el target (vidrio de sílice)
        G4double innerTargetRadius = 0.0;
        G4double outerTargetRadius = 2.5 * cm;

        G4Tubs* solidTarget = new G4Tubs("Target", innerTargetRadius, outerTargetRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
        G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, target, "Target");

        // Posicionar el target en el mundo
        G4ThreeVector targetPos = G4ThreeVector(); // Posición (0,0,0)
        G4VPhysicalVolume* fTargetLog = new G4PVPlacement(nullptr, targetPos, logicTarget, "Target", fWorldLog, false, 0);

        // Crear el detector (plomo tungsteno)
        G4double detectorSizeXY = 20 * cm;
        G4double detectorSizeZ = 10 * cm;
        auto solidDetector = new G4Box("Detector", detectorSizeXY, detectorSizeXY, detectorSizeZ);
        fDetectorLog = new G4LogicalVolume(solidDetector, E_PbWO4, "Detector");

        // Posicionar el detector en el mundo
        G4ThreeVector detectorPos = G4ThreeVector(0, 0, 20 * cm);
        new G4PVPlacement(nullptr, detectorPos, fDetectorLog, "Detector", fWorldLog, false, 0);

        // Definir propiedades ópticas y asignarlas a la superficie
        DefineOpticalProperties();
        G4OpticalSurface* opticalSurface = new G4OpticalSurface("OpticalSurface");
        opticalSurface->SetType(dielectric_dielectric);
        opticalSurface->SetFinish(polished);
        opticalSurface->SetModel(unified);

        auto* surface = new G4LogicalBorderSurface("Surface", physWorld, fTargetLog, opticalSurface);

        // Definir este detector como el detector gamma
        fGammaDetector = fDetectorLog;

        return physWorld;
    }

    void DetectorConstruction::SetTargetThickness(G4double thickness)
    {
        fTargetThickness = thickness;
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}
