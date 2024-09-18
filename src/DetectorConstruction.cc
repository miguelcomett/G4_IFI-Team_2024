#include "DetectorConstruction.hh"

namespace G4_PCM
{
    // Constructor
    DetectorConstruction::DetectorConstruction()
        : fTargetThickness(50 * mm), // Valor predeterminado de grosor del objetivo
        fMessenger(new DetectorConstructionMessenger(this)) // Crear el mensajero
    {
        DefineMaterials();

        // Selección de arquitectura del objetivo -SELECCIONA UNA-
        isArm = true;        // Brazo
        isSingleBone = false; // Solo hueso

        // Tipo de hueso -SELECCIONA UNO-
        isNormalBone = false;  // Hueso sólido
        isRealisticBone = true; // Hueso sólido y trabecular

        // ¿Quieres que tu hueso tenga osteoporosis?
        isOsBone = true;  // Hueso osteoporótico

        // Filtro para eliminar ruido
        isFiltered = false; // Filtro para dejar pasar solo la parte circular

        // Radio total del hueso
        outerBoneRadius = isRealisticBone ? 2.25 * cm : 1.5 * cm; // Selección del radio en función del tipo de hueso
        innerBoneRadius = 0.0;

        // Rotación del objetivo
        targetRotation = new G4RotationMatrix(0, 90 * deg, 0);

        // Tamaño del detector
        detectorSizeXY = 20 * cm;
        detectorSizeZ = 5 * cm;

        // Parámetros del filtro
        filterThick = 0.5 * cm;
    }

    // Destructor
    DetectorConstruction::~DetectorConstruction()
    {
        delete fMessenger; // Eliminar el mensajero
    }

    // Definir materiales
    void DetectorConstruction::DefineMaterials()
    {
        G4NistManager* nist = G4NistManager::Instance();

        // Materiales básicos
        bone = nist->FindOrBuildMaterial("G4_BONE_CORTICAL_ICRP");
        muscle = nist->FindOrBuildMaterial("G4_MUSCLE_SKELETAL_ICRP");
        skin = nist->FindOrBuildMaterial("G4_SKIN_ICRP");
        grasa = nist->FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
        vacuum = nist->FindOrBuildMaterial("G4_AIR");

        // Material para los cristales de PbWO4
        E_PbWO4 = new G4Material("E_PbWO4", 8.28 * g / cm3, 3);
        E_PbWO4->AddElement(nist->FindOrBuildElement("Pb"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("W"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("O"), 4);

        // Material para hueso osteoporótico
        H = nist->FindOrBuildMaterial("G4_H");
        C = nist->FindOrBuildMaterial("G4_C");
        N = nist->FindOrBuildMaterial("G4_N");
        O = nist->FindOrBuildMaterial("G4_O");
        Mg = nist->FindOrBuildMaterial("G4_Mg");
        P = nist->FindOrBuildMaterial("G4_P");
        S = nist->FindOrBuildMaterial("G4_S");
        Ca = nist->FindOrBuildMaterial("G4_Ca");
        OsBone = new G4Material("OsteoporoticBone", 1.3 * g / cm3, 8);
        OsBone->AddMaterial(H, 6.4 * perCent);
        OsBone->AddMaterial(C, 27.8 * perCent);
        OsBone->AddMaterial(N, 2.7 * perCent);
        OsBone->AddMaterial(O, 41 * perCent);
        OsBone->AddMaterial(Mg, 0.2 * perCent);
        OsBone->AddMaterial(P, 7 * perCent);
        OsBone->AddMaterial(S, 0.2 * perCent);
        OsBone->AddMaterial(Ca, 14.7 * perCent);

        // Material para hueso normal realista
        trabecularBone = nist->FindOrBuildMaterial("G4_B-100_BONE");
        F = nist->FindOrBuildMaterial("G4_F");
        RealOsBone = new G4Material("RealOsteoporoticBone", 1.3 * g / cm3, 6);
        RealOsBone->AddMaterial(H, 6.54709 * perCent);
        RealOsBone->AddMaterial(C, 53.6944 * perCent);
        RealOsBone->AddMaterial(N, 2.15 * perCent);
        RealOsBone->AddMaterial(O, 3.2085 * perCent);
        RealOsBone->AddMaterial(F, 16.7411 * perCent);
        RealOsBone->AddMaterial(Ca, 17.6589 * perCent);
    }

    // Construir hueso osteoporótico con poros
    void DetectorConstruction::ConstructOsBone()
    {
        G4double poreRadius = 100 * um;
        pore = new G4Sphere("Pore", 0, poreRadius, 0. * deg, 360. * deg, 0. * deg, 180. * deg);

        int numPores = 300;
        porousBone = isNormalBone ? solidBone : solidTrabecular;

        for (int i = 1; i < numPores; ++i)
        {
            G4double r = G4UniformRand() * (isNormalBone ? outerBoneRadius : (outerBoneRadius - 0.25));
            G4double theta = G4UniformRand() * 360.0 * deg;
            G4double z = G4UniformRand() * fTargetThickness - fTargetThickness / 2;

            G4ThreeVector porePosition(r * std::cos(theta), r * std::sin(theta), z);
            porousBone = new G4SubtractionSolid("PorousBone", porousBone, pore, 0, porePosition);
        }

        G4LogicalVolume* logicBone = isNormalBone ?
            new G4LogicalVolume(porousBone, OsBone, "PorousBoneLogical") :
            new G4LogicalVolume(porousBone, RealOsBone, "PorousBoneLogical");

        new G4PVPlacement(targetRotation, targetPos, logicBone, isNormalBone ? "physBone" : "physTrabecular", logicWorld, false, 0);
    }

    // Construir hueso realista (trabecular y cortical)
    void DetectorConstruction::ConstructRealBone()
    {
        G4double outerTrabecularRadius = outerBoneRadius - 0.25 * cm;
        solidTrabecular = new G4Tubs("TrabecularBone", innerBoneRadius, outerTrabecularRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
        solidCortical = new G4Tubs("CorticalBone", outerTrabecularRadius, outerBoneRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);

        if (isOsBone)
        {
            ConstructOsBone();
        }
        else
        {
            logicTrabecular = new G4LogicalVolume(solidTrabecular, trabecularBone, "logicTrabecular");
            new G4PVPlacement(targetRotation, targetPos, logicTrabecular, "physTrabecular", logicWorld, false, 0);

            logicCortical = new G4LogicalVolume(solidCortical, bone, "LogicCortical");
            new G4PVPlacement(targetRotation, targetPos, logicCortical, "physCortical", logicWorld, false, 0);
        }
    }

    // Construir hueso normal
    void DetectorConstruction::ConstructNormalBone()
    {
        solidBone = new G4Tubs("Bone", innerBoneRadius, outerBoneRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);

        if (isOsBone)
        {
            ConstructOsBone();
        }
        else
        {
            logicBone = new G4LogicalVolume(solidBone, bone, "LogicBone");
            new G4PVPlacement(targetRotation, targetPos, logicBone, "physBone", logicWorld, false, 0, true);
        }
    }

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
        new G4PVPlacement(targetRotation, targetPos, logicMuscle, "physMuscle", logicWorld, false, 0, true);
        new G4PVPlacement(targetRotation, targetPos, logicGrasa, "physGrasa", logicWorld, false, 0, true);
        new G4PVPlacement(targetRotation, targetPos, logicSkin, "physSkin", logicWorld, false, 0, true);

        // Construcción del hueso correspondiente
        if (isNormalBone)
        {
            ConstructNormalBone();
        }
        else if (isRealisticBone)
        {
            ConstructRealBone();
        }
    }

    // Método para construir el detector
    G4VPhysicalVolume* DetectorConstruction::Construct()
    {
        // Crear el mundo
        solidWorld = new G4Box("solidWorld", detectorSizeXY / 2.0, detectorSizeXY / 2.0, detectorSizeZ / 2.0);
        logicWorld = new G4LogicalVolume(solidWorld, vacuum, "logicWorld");
        physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "physWorld", 0, false, 0, true);

        // Construcción del brazo
        if (isArm)
        {
            ConstructArm();
        }
        else if (isNormalBone)
        {
            ConstructNormalBone();
        }
        else if (isRealisticBone)
        {
            ConstructRealBone();
        }

        return physWorld;
    }

    // Modificar el grosor del objetivo
    void DetectorConstruction::SetTargetThickness(G4double thickness)
    {
        fTargetThickness = thickness;
    }
}
