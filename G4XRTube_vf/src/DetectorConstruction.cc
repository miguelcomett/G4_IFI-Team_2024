//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "SensitiveDetector.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Para.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"


#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(), worldPV(0),
      fWorldMater(0), fAnodeMaterial(0), fFilterMaterial(0),
      vacuum(0), lead(0), air(0), pyrex(0), CdTe(0), NaI(0),
      detectorLV(0), anodeLV(0), filterLV(0),worldLV(0),
      fDetectorMessenger(0),
      fCheckOverlaps(true)
{
    fAnodeAngle = 10. * deg;
    fFilterThickness = 1.3 * mm;
    DefineMaterials();
    fDetectorMessenger = new DetectorMessenger(this);
    
    //Create simple arm
    outerBoneRadius = 2.25 * cm; 
    innerBoneRadius = 0.0 * cm; 
    targetRotation = new G4RotationMatrix(0. *deg, -90 *deg, 0. *deg);
    fTargetThickness = 50 *mm; 
    targetX = 0.0 * cm; 
    targetY = 0.0 * cm; 
    targetZ = -92 *cm; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete fDetectorMessenger;
}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    return ConstructVolumes();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Construir brazo con músculo, grasa y piel
void DetectorConstruction::ConstructArm()
{
    // Radios de los diferentes tejidos
    G4double innerMuscleRadius = outerBoneRadius;
    G4double outerMuscleRadius = innerMuscleRadius + 2.5 * cm;
    G4double innerGrasaRadius = outerMuscleRadius;
    G4double outerGrasaRadius = innerGrasaRadius + 0.5 * cm;
    G4double innerSkinRadius = outerGrasaRadius;
    G4double outerSkinRadius = innerSkinRadius + 0.15 * cm;

    // Creación de sólidos para cada tejido
    solidMuscle = new G4Tubs("Muscle", innerMuscleRadius, outerMuscleRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
    solidGrasa = new G4Tubs("Grasa", innerGrasaRadius, outerGrasaRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
    solidSkin = new G4Tubs("Skin", innerSkinRadius, outerSkinRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);

    // Volumen lógico y colocación para cada tejido
    G4LogicalVolume* logicMuscle = new G4LogicalVolume(solidMuscle, muscle, "LogicMuscle");
    G4LogicalVolume* logicGrasa = new G4LogicalVolume(solidGrasa, grasa, "LogicGrasa");
    G4LogicalVolume* logicSkin = new G4LogicalVolume(solidSkin, skin, "LogicSkin");

// Colocación en el mundo lógico
    new G4PVPlacement(targetRotation, G4ThreeVector(targetX, targetY, targetZ), logicMuscle, "physMuscle", worldLV, false, 0, fCheckOverlaps);
    new G4PVPlacement(targetRotation, G4ThreeVector(targetX, targetY, targetZ), logicGrasa, "physGrasa", worldLV, false, 0, fCheckOverlaps);
    new G4PVPlacement(targetRotation, G4ThreeVector(targetX, targetY, targetZ), logicSkin, "physSkin", worldLV, false, 0, fCheckOverlaps);

// Construcción del hueso correspondiente
    ConstructRealBone();
}

// Construir hueso realista (trabecular y cortical)
void DetectorConstruction::ConstructRealBone()
{
    G4double outerTrabecularRadius = outerBoneRadius - 0.25 * cm;
    solidTrabecular = new G4Tubs("TrabecularBone", innerBoneRadius, outerTrabecularRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
    solidCortical = new G4Tubs("CorticalBone", outerTrabecularRadius, outerBoneRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);

    G4LogicalVolume *logicTrabecular = new G4LogicalVolume(solidTrabecular, trabecularBone, "logicTrabecular");
    new G4PVPlacement(targetRotation, G4ThreeVector(targetX, targetY, targetZ), logicTrabecular, "physTrabecular", worldLV,false, 0, fCheckOverlaps);

    G4LogicalVolume *logicCortical = new G4LogicalVolume(solidCortical, bone, "LogicCortical");
    new G4PVPlacement(targetRotation, G4ThreeVector(targetX, targetY, targetZ), logicCortical, "physCortical", worldLV, false,0, fCheckOverlaps);

}

void DetectorConstruction::DefineMaterials()
{
    // Lead material defined using NIST Manager
    auto nistManager = G4NistManager::Instance();

    fWorldMater = nistManager->FindOrBuildMaterial("G4_AIR");

    fAnodeMaterial = nistManager->FindOrBuildMaterial("G4_W");

    lead = nistManager->FindOrBuildMaterial("G4_Be"); // be

    air = nistManager->FindOrBuildMaterial("G4_AIR");
    pyrex = nistManager->FindOrBuildMaterial("G4_Pyrex_Glass");

    G4double a; // mass of a mole;
    G4double z; // z=mean number of protons;
    G4double density;

    // Vacuum
    vacuum = new G4Material("Galactic", z = 1., a = 1.01 * g / mole, density = universe_mean_density,
                            kStateGas, 2.73 * kelvin, 3.e-18 * pascal);

    G4Element *Cd = new G4Element("Cadmium", "Cd", 48., 112.40 * g / mole);
    G4Element *Te = new G4Element("Telliurium", "Te", 52., 127.60 * g / mole);

    CdTe = new G4Material("CdTe", 5.85 * g / cm3, 2);
    CdTe->AddElement(Cd, 1);
    CdTe->AddElement(Te, 1);

    NaI = nistManager->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    fFilterMaterial = vacuum;
    // Print materials
    // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
    // Materiales del brazo
    muscle = nistManager->FindOrBuildMaterial("G4_MUSCLE_SKELETAL_ICRP");
    skin = nistManager->FindOrBuildMaterial("G4_SKIN_ICRP");
    grasa = nistManager->FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
    trabecularBone = nistManager->FindOrBuildMaterial("G4_B-100_BONE");
    bone = nistManager->FindOrBuildMaterial("G4_BONE_CORTICAL_ICRP");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume *DetectorConstruction::ConstructVolumes()
{
    // if (worldPV)
    // {
    //     return worldPV;
    // }

    // Cleanup old geometry
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    //
    // World
    //
    G4double worldSizeXYZ = 500. * cm;
    G4Box *solidWorld = new G4Box("World",                                               // its name
                                  worldSizeXYZ / 2, worldSizeXYZ / 2, worldSizeXYZ / 2); // its size

    worldLV = new G4LogicalVolume(
        solidWorld, // its solid
        vacuum,     // its material
        "World");   // its name

    worldPV = new G4PVPlacement(
        0,               // no rotation
        G4ThreeVector(), // at (0,0,0)
        worldLV,         // its logical volume
        "World",         // its name
        0,               // its mother  volume
        false,           // no boolean operation
        0,               // copy number
        fCheckOverlaps); // checking overlaps

    //
    // Be filter
    //
    G4Box *inhfilterSolid =
        new G4Box("inhfilter",                           // its name
                  10 / 2. * cm, 10 / 2. * cm, 0.8 / 2.); // its size

    G4LogicalVolume *inhfilterLV =
        new G4LogicalVolume(inhfilterSolid, // its solid
                            vacuum,         // its material
                            "inhfilter");   // its name

    new G4PVPlacement(0,                        // no rotation
                      G4ThreeVector(0, 0, -30), // at (0,0,0)
                      inhfilterLV,              // its logical volume
                      "inhfilter",              // its name
                      worldLV,                  // its mother  volume
                      false,                    // no boolean operation
                      0,                        // copy number
                      fCheckOverlaps);          // overlaps checking

    //
    // filter
    //
    G4Box *solidfilter =
        new G4Box("filter",                                           // its name
                  60 / 2. * cm, 60 / 2. * cm, fFilterThickness / 2.); // its size

    filterLV =
        new G4LogicalVolume(solidfilter,     // its solid
                            fFilterMaterial, // its material
                            "filter");       // its name

    new G4PVPlacement(0,                        // no rotation
                      G4ThreeVector(0, 0, -40), // at (0,0,0)
                      filterLV,                 // its logical volume
                      "filter",                 // its name
                      worldLV,                  // its mother  volume
                      false,                    // no boolean operation
                      0,                        // copy number
                      fCheckOverlaps);          // overlaps checking
    //Construct the arm
    ConstructArm(); 
    // Anode: ...
    // anode parameters:
    //
	
    // G4double alpha = 18. * deg;
    G4double theta = 0 * deg;
    G4double phi = 0 * deg;
    // G4double anodeAngle = 15. * deg;

    G4Para *solidAnode = new G4Para("Anode", 10 * mm, 5 * mm, 5 * mm, fAnodeAngle, theta, phi);
    anodeLV = new G4LogicalVolume(solidAnode,     // its solid
                                  fAnodeMaterial, // its material
                                  "Anode");       // its name

    G4RotationMatrix *rotAnode = new G4RotationMatrix();
    rotAnode->rotateX(0. * deg);
    rotAnode->rotateY(-90. * deg);
    rotAnode->rotateZ(-90. * deg);

    new G4PVPlacement(rotAnode,                       // no rotation!!!!! with rotation
                      G4ThreeVector(0, -10., 0 * mm), // at (0,0,0)
                      anodeLV,                        // its logical volume
                      "Anode",                        // its name
                      worldLV,                        // its mother  volume
                      false,                          // no boolean operation
                      0,                              // copy number
                      fCheckOverlaps);

    //
    // Detecto volume
    //
    G4double RminDetector(0. * mm), RmaxDetector(120. * mm), HzDetector(1 * mm);

    G4Tubs *solidDetector =
        new G4Tubs("detector",
                   RminDetector,    // innerRadius
                   RmaxDetector,    // outerRadius
                   HzDetector / 2., // hz
                   0. * deg,        // startAngle
                   360 * deg);      // spanningAngle

    detectorLV =
        new G4LogicalVolume(solidDetector, // its solid
                            vacuum,        // its material
                            "detector");   // its name

    new G4PVPlacement(0, // no rotation
                      G4ThreeVector(0,
                                    0,
                                    -100 * cm), // at (0,0,0)
                      detectorLV,              // its logical volume
                      "detector",              // its name
                      worldLV,                 // its mother volume
                      false,                   // no boolean operation
                      0,                       // copy number
                      fCheckOverlaps);         // overlaps checking

    // Visualization attributes
    //
    G4VisAttributes *blue = new G4VisAttributes(G4Color(0., 0., 1., 0.1));
    blue->SetVisibility(true);
    blue->SetForceSolid(true);

    G4VisAttributes *green = new G4VisAttributes(G4Color(0., 1., 0., 0.1));
    green->SetVisibility(true);
    green->SetForceSolid(true);

    G4VisAttributes *black = new G4VisAttributes(G4Color(0., 0., 0., 1.));
    black->SetVisibility(true);
    black->SetForceSolid(true);

    G4VisAttributes *white = new G4VisAttributes(G4Color(1., 1., 1., 1.));
    white->SetVisibility(true);
    white->SetForceSolid(true);

    G4VisAttributes *red = new G4VisAttributes(G4Color(1., 0., 0., 1.));
    red->SetVisibility(true);
    red->SetForceSolid(false);

    //worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());

    anodeLV->SetVisAttributes(black);
    detectorLV->SetVisAttributes(black);
    filterLV->SetVisAttributes(blue);
    inhfilterLV->SetVisAttributes(blue);

    //
    // Always return the physical World
    //
    return worldPV;
}
void DetectorConstruction::SetAodeMaterial(G4String materialChoice)
{
    // search the material by its name
    G4Material *pttoMaterial =
        G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

    if (pttoMaterial)
    {
        fAnodeMaterial = pttoMaterial;
        if (anodeLV)
        {
            anodeLV->SetMaterial(fAnodeMaterial);
        }
        G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    }
    else
    {
        G4cout << "\n--> warning from DetectorConstruction::SetAodeMaterial : "
               << materialChoice << " not found" << G4endl;
    }
}
G4Material *DetectorConstruction::GetTargetMaterial()
{
    return fAnodeMaterial;
}
void DetectorConstruction::SetFilterMaterial(G4String materialChoice)
{
    // search the material by its name
    G4Material *pttoMaterial =
        G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

    if (pttoMaterial)
    {
        fFilterMaterial = pttoMaterial;
        if (filterLV)
        {
            filterLV->SetMaterial(fFilterMaterial);
        }
        G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    }
    else
    {
        G4cout << "\n--> warning from DetectorConstruction::SetFilterMaterial : "
               << materialChoice << " not found" << G4endl;
    }
}

G4Material *DetectorConstruction::GetFilterMaterial()
{
    return fFilterMaterial;
}

void DetectorConstruction::SetAnodeAngle(G4double value)
{
    fAnodeAngle = value;
    G4RunManager::GetRunManager()->ReinitializeGeometry();
    // G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}
G4double DetectorConstruction::GetAnodeAngle()
{
    return fAnodeAngle;
}
void DetectorConstruction::SetFilterThickness(G4double value)
{
    fFilterThickness = value;
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}
G4double DetectorConstruction::GetFilterThickness()
{
    return fFilterThickness;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    // SensitiveDetector *sensDet = new SensitiveDetector("SensitiveDetector");

    // detectorLV->SetSensitiveDetector(sensDet);
    SensitiveDetector *SD = new SensitiveDetector("SD", "DetectorHitsCollection");
    G4SDManager::GetSDMpointer()->AddNewDetector(SD);
    // SetSensitiveDetector("detector", SD);
    detectorLV->SetSensitiveDetector(SD);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
