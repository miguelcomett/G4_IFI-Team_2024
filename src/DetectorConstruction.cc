#include "DetectorConstruction.hh"

namespace G4_PCM
{
    // Constructor
    DetectorConstruction::DetectorConstruction()
        : fTargetThickness(10 * mm), // Valor predeterminado de grosor del objetivo 10k joya
        fMessenger(new DetectorConstructionMessenger(this)) // Crear el mensajero
    {
        // Crear una instancia de STLGeometryReader
        stlReader = new STLGeometryReader();

        DefineMaterials();

        // Selección de arquitectura del objetivo -SELECCIONA UNA-
        isRealHand = false;    // Mano Realista 3D
        isArm = false;        // Brazo
        isSingleBone = false; // Solo hueso

        // Tipo de hueso -SELECCIONA UNO-
        isNormalBone = false;  // Hueso sólido
        isRealisticBone = false; // Hueso sólido y trabecular
        isBoneWall = false; //Pared de hueso 
        isArmWall = false; //Pared de braso tissue

        // ¿Quieres que tu hueso tenga osteoporosis?
        isOsBone = false;  // Hueso osteoporótico

        // Filtro para eliminar ruido
        isFiltered = false; // Filtro para dejar pasar solo la parte circular

        // Radio total del hueso
        outerBoneRadius = isRealisticBone ? 2.25 * cm : 1.5 * cm; // Selección del radio en función del tipo de hueso
        innerBoneRadius = 0.0;

        // Rotación del objetivo
        targetRotation = new G4RotationMatrix(0, -90 * deg, 0);

        // Tamaño del detector
        detectorSizeXY = 35 * cm;
        detectorSizeZ = 5 * cm;

        // Parámetros del filtro
        filterThick = 0.5 * cm;
    }

    // Destructor
    DetectorConstruction::~DetectorConstruction()
    {
        // Liberar memoria
        delete stlReader;
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
        //vacuum = nist->FindOrBuildMaterial("G4_AIR");
        vacuum = nist->FindOrBuildMaterial("G4_Galactic");
        tungsten = nist->FindOrBuildMaterial("G4_W");
        silicon = nist->FindOrBuildMaterial("G4_Si");
        
        // Material para el sólido STL
        material3D = nist->FindOrBuildMaterial("G4_B-100_BONE");
        material3Dsoft = nist->FindOrBuildMaterial("G4_TISSUE_SOFT_ICRU-4");
        //material3Dsoft = nist->FindOrBuildMaterial("G4_AIR");

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

    // Construir walls
    void DetectorConstruction::ConstructBoneWall()
    {
    	G4double outerTrabecularRadius = 2.0*cm; 
    	solidHuesoTrabecular = new G4Box("FIrstBoneWall", detectorSizeXY, detectorSizeXY, outerTrabecularRadius); 
    	solidHuesoCortical = new G4Box("SecondBoneWall", detectorSizeXY, detectorSizeXY, 0.25*cm);
    	
    	logicHuesoTrabecular = new G4LogicalVolume(solidHuesoTrabecular, trabecularBone, "firstLogicWall"); 
    	logicHuesoCortical = new G4LogicalVolume(solidHuesoCortical, bone, "secondWallLogic"); 
    	
    	physHuesoCortical = new G4PVPlacement(targetRotation, targetPos, logicHuesoCortical, "secondPhysWall", logicWorld, false, 0, true);  
    	physHuesoTrabecular = new G4PVPlacement(targetRotation, G4ThreeVector(0, 0, 2.25*cm), logicHuesoTrabecular, "firstPhysWall", logicWorld, false, 0, true);
    
    }
    
    void DetectorConstruction::ConstructArmWall()
    {
        //solidSkinP =    new G4Box("SolidSkinP",     0.4*m, 0.4*m, 0.15*cm);
        //solidFatP =     new G4Box("SolidFatP",      0.4*m, 0.4*m, 0.5*cm);
        //solidMuscleP =  new G4Box("SolidMuscleP",   0.4*m, 0.4*m, 2.5*cm);

        solidSkinP = new G4Box("SolidSkinP", 0.4 * m, 0.4 * m, 0.15 * cm);
        solidFatP = new G4Box("SolidFatP", 0.4 * m, 0.4 * m, (0.5 * 2*5) * cm);
        solidMuscleP = new G4Box("SolidMuscleP", 0.4 * m, 0.4 * m, 2.5 * cm);

        logicSkinP = new G4LogicalVolume(solidSkinP, skin, "LogicSkinP");
        logicFatP = new G4LogicalVolume(solidFatP, grasa, "LogicFatP");
        logicMuscleP = new G4LogicalVolume(solidMuscleP, muscle, "LogicMuscleP");

        //G4ThreeVector pos1(0 * cm, 0 * cm, (0) * cm);
        //G4ThreeVector pos2(0 * cm, 0 * cm, (0.65 * cm);
        //G4ThreeVector pos3(0 * cm, 0 * cm, (3.65 * cm);

        G4ThreeVector pos1(0*cm, 0*cm, (0)*cm);
        G4ThreeVector pos2(0*cm, 0*cm, (5.15)*cm);
        G4ThreeVector pos3(0*cm, 0*cm, (8+4.65)*cm);

        physSkinP = new G4PVPlacement(targetRotation, pos1, logicSkinP, "physSkinP", logicWorld, false, 0, true);
        physFatP = new G4PVPlacement(targetRotation, pos2, logicFatP, "physFatP", logicWorld, false, 0, true);
        physMuscleP = new G4PVPlacement(targetRotation, pos3, logicMuscleP, "physMuscleP", logicWorld, false, 0, true);

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
            ConstructOsBone(); //Construir hueso con osteo
        }
        else //HUeso sin osteo
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

    // Método para construir mano con base en modelo STL 3D
    void DetectorConstruction::ConstructBONE3D()
    {
        //// Crear el sólido a partir del archivo STL
        G4STL stl; 
        stlSolid = stl.Read("C:\\Users\\conej\\Documents\\Universidad\\Geant4\\Projects\\Models\\BoneLAST.stl");

        if (stlSolid) {
            // Crear volumen lógico
            G4LogicalVolume* logicSTL = new G4LogicalVolume(stlSolid, material3D, "STLModelLogical");
            
            // Colocar el modelo en el mundo
            new G4PVPlacement(targetRotation, targetPos, logicSTL, "STLModelPhysical", logicWorld, false, 0, true);

            G4cout << "Modelo bone importado exitosamente" << G4endl;
        }
        else {
            G4cout << "Modelo bone no importado" << G4endl;
        }
    }

    void DetectorConstruction::ConstructSOFT3D()
    {
        //// Crear el sólido a partir del archivo STL
        G4STL stl;
        stlSolid2 = stl.Read("C:\\Users\\conej\\Documents\\Universidad\\Geant4\\Projects\\Models\\HAND-HAND-BOOL.stl");

        G4ThreeVector posXD(0 * cm, 0 * cm, -7 * cm);

        if (stlSolid2) {
            // Crear volumen lógico
            G4LogicalVolume* logicSTL2 = new G4LogicalVolume(stlSolid2, material3Dsoft, "STLModelLogical2");

            // Colocar el modelo en el mundo
            new G4PVPlacement(targetRotation, posXD, logicSTL2, "STLModelPhysical2", logicWorld, false, 0, true);

            G4cout << "Modelo tissue importado exitosamente" << G4endl;
        }
        else {
            G4cout << "Modelo tissue no importado" << G4endl;
        }
    }

    void DetectorConstruction::ConstructSOFT3Dbool()
    {
        //// Crear el sólido a partir del archivo STL del modelo "tissue"
        G4STL stl;
        stlSolid2 = stl.Read("C:\\Users\\conej\\Documents\\Universidad\\Geant4\\Projects\\Models\\HAND-HAND.stl");

        //// Crear el sólido a partir del archivo STL del modelo "bone"
        stlSolid = stl.Read("C:\\Users\\conej\\Documents\\Universidad\\Geant4\\Projects\\Models\\BoneLAST.stl");

        if (stlSolid && stlSolid2) {
            // Resta el volumen "bone" del volumen "tissue"
            G4SubtractionSolid* subtractedSolid = new G4SubtractionSolid("SoftWithBoneHole", stlSolid2, stlSolid, targetRotation, targetPos);

            // Crear el volumen lógico del sólido resultante
            G4LogicalVolume* logicSubtracted = new G4LogicalVolume(subtractedSolid, material3Dsoft, "STLModelLogicalSubtracted");
            G4ThreeVector posXD1(0 * cm, 0 * cm, -7 * cm);
            // Colocar el sólido resultante en el mundo
            new G4PVPlacement(targetRotation, posXD1, logicSubtracted, "STLModelPhysicalSubtracted", logicWorld, false, 0, true);

            G4cout << "Modelo tissue con hueco de bone importado exitosamente" << G4endl;
        }
        else {
            G4cout << "Modelo bone o tissue no importado correctamente" << G4endl;
        }
    }

    // Construir una placa de tungsteno
    void DetectorConstruction::ConstructTungstenPlate()
    {
        // Dimensiones de la placa (ancho, alto, grosor)
        G4double plateWidth = 10.0 * cm;
        G4double plateHeight = fTargetThickness;
        G4double plateThickness = 10.0 * cm;

        // Crear la geometría de la placa (una caja)
        G4Box* solidTungstenPlate = new G4Box("TungstenPlate", plateWidth / 2.0, plateHeight / 2.0, plateThickness / 2.0);

        // Definir el volumen lógico de la placa usando el material tungsteno
        G4LogicalVolume* logicTungstenPlate = new G4LogicalVolume(solidTungstenPlate, tungsten, "LogicTungstenPlate");

        // Crear rotación en una línea (45 grados en X, 30 en Y, 60 en Z)
        plateRotation = new G4RotationMatrix(0, -17 * deg, 0);

        // Posición de la placa
        G4ThreeVector platePosition(0, 0, 0); // Posición de la placa

        // Colocar el volumen físico de la placa en el mundo lógico
        new G4PVPlacement(plateRotation, platePosition, logicTungstenPlate, "physTungstenPlate", logicWorld, false, 0);
    }

    // Construir una caja de vacío (cubo de 5x5x5 cm)
    void DetectorConstruction::ConstructVacuumBox()
    {
        G4double vacuumSize = 50.0 * cm;
        G4Box* solidVacuumBox = new G4Box("VacuumBox", vacuumSize / 2.0, vacuumSize / 2.0, vacuumSize / 2.0);
        G4LogicalVolume* logicVacuumBox = new G4LogicalVolume(solidVacuumBox, vacuum1, "LogicVacuumBox");
        G4ThreeVector vacuumPosition(0, 0, 7*cm);
        // new G4PVPlacement(0, vacuumPosition, logicVacuumBox, "physVacuumBox", logicWorld, false, 0);

        G4ThreeVector vacuumPosition1(0, 0, 7*cm);
        BoxRotation = new G4RotationMatrix(0, 0 * deg, 0);
        G4double tungstenBoxSize = vacuumSize*1.1;
        G4Box* solidTungstenBox = new G4Box("TungstenBox", tungstenBoxSize / 2.0, tungstenBoxSize / 2.0, tungstenBoxSize / 2.0);
        G4SubtractionSolid* solidTungstenShell = new G4SubtractionSolid("TungstenShell", solidTungstenBox, solidVacuumBox, BoxRotation, vacuumPosition1);
        G4LogicalVolume* logicTungstenShell = new G4LogicalVolume(solidTungstenShell, tungsten, "LogicTungstenShell");
        new G4PVPlacement(0, vacuumPosition1, logicTungstenShell, "physTungstenShell", logicWorld, false, 0);
    }



    // Método para construir el detector
    G4VPhysicalVolume* DetectorConstruction::Construct()
    {
        // Definir el tamaño del mundo
        G4double worldSize = 1 * m;
        solidWorld = new G4Box("World",
            worldSize / 2,
            worldSize / 2,
            worldSize * 10);
        logicWorld = new G4LogicalVolume(solidWorld,
            vacuum,
            "World");
        physWorld = new G4PVPlacement(nullptr,
            G4ThreeVector(),
            logicWorld,
            "World",
            nullptr,
            false,
            0);


        ConstructTungstenPlate();
        ConstructVacuumBox();

        // Construcción del brazo
        if (isRealHand)
        {
            ConstructSOFT3Dbool();
            ConstructBONE3D();
            //ConstructSOFT3D();
        }
        else if (isArm)
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
        else if (isBoneWall)
        {
            ConstructBoneWall(); 
        }
        else if (isArmWall)
        {
            ConstructArmWall(); 
        }
        
    

        // Construir el detector
        G4Box* solidDetector = new G4Box(
            "Detector",
            detectorSizeXY,
            detectorSizeXY,
            detectorSizeZ);

        G4LogicalVolume* logicDetector = new G4LogicalVolume(
            solidDetector,
            //E_PbWO4,
            silicon,
            "Detector");

        G4ThreeVector detectorPos = G4ThreeVector(0, 0, 100 * cm); // Era 20
        G4RotationMatrix* detRotation = new G4RotationMatrix();

        // Colocar el detector
        new G4PVPlacement(detRotation,
            detectorPos,
            logicDetector,
            "Detector",
            logicWorld,
            false,
            0);

        // Definir este detector como el detector gamma
        fGammaDetector = logicDetector;
        fBoxDestroyer = logicTungstenShell;
        //fBoxDestroyer = logicWorld;

        return physWorld;
    }

    // Modificar el grosor del objetivo
    void DetectorConstruction::SetTargetThickness(G4double thickness)
    {
        fTargetThickness = thickness;
    }
}
