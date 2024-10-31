#include "3.0_DetectorConstruction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
    DefineMaterials();
    stlReader = new STLGeometryReader();

    fDetectorMessenger = new G4GenericMessenger(this, "/myDetector/", "Detector Construction");
    fDetectorMessenger -> DeclareProperty("nColumns", DetColumnNum, "Number of columns");
    fDetectorMessenger -> DeclareProperty("nRows", DetRowNum, "Number of rows");
    fDetectorMessenger -> DeclareProperty("ThicknessTarget", target_Thickness, "Thickness of the target");

    DetColumnNum = 10, DetRowNum = 10;

    target_Thickness = 1 * mm; 

    boneHeight = 60 * mm;
    innerBoneRadius = 0.0;
    outerBoneRadius = 22.5 * mm;
    targetRotation = new G4RotationMatrix(0, 90*deg, 0);
    targetPosition = G4ThreeVector(0.0, 0.0, 0.0);

    isArm = false;
    isBone = false;
    isOsBone = false;
    isArmDivided = false;
    is3DModel = true;
}

MyDetectorConstruction::~MyDetectorConstruction(){}

G4VPhysicalVolume * MyDetectorConstruction::Construct()
{
    xWorld = 0.5*m, yWorld = 0.5*m, zWorld = 0.5*m;
    solidWorld = new G4Box("SolidWorld", xWorld, yWorld, zWorld);
    logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "LogicalWorld");
    physicalWorld = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), logicWorld, "PhysicalWorld", 0, false, 0, true);

    if (isArm) ConstructArm(); else if (isBone) ConstructBone(); else if (is3DModel) ConstructThorax(); else ConstructTarget();

    solidDetector = new G4Box("solidDetector", xWorld/DetRowNum, yWorld/DetColumnNum, 0.01*m);
    logicDetector = new G4LogicalVolume(solidDetector, Silicon, "logicalDetector");
    check_Overlaps = false;
    for(G4int i = 0; i < DetRowNum; i++) for (G4int j = 0; j < DetColumnNum; j++)
    {
        DetectorPosition = G4ThreeVector(-0.5*m + (i+0.5)*m/DetRowNum, -0.5*m + (j+0.5)*m/DetColumnNum, 0.49*m);
        physicalDetector = new G4PVPlacement(0, DetectorPosition, logicDetector, "physicalDetector", logicWorld, false, j+(i*DetColumnNum), check_Overlaps);
    }
    if (isArm || isBone) fScoringVolume = logicDetector;

    return physicalWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector * sensitiveDetector = new MySensitiveDetector("sensitiveDetector");
    logicDetector -> SetSensitiveDetector(sensitiveDetector);
}

// Build Geometries ===============================================================================================================================

void MyDetectorConstruction::ConstructTarget()
{ 
    materialTarget = Bone;

    Radiator_Position = G4ThreeVector(0.0, 0.0, 0.25*m);

    solidRadiator = new G4Box("solidRadiator", 0.25*m, 0.25*m, target_Thickness/2);
    logicRadiator = new G4LogicalVolume(solidRadiator, materialTarget, "logicalRadiator");
    physicalRadiator = new G4PVPlacement(0, Radiator_Position, logicRadiator, "PhysicalRadiator", logicWorld, false, 0, true);
    fScoringVolume = logicRadiator;
}

void MyDetectorConstruction::ConstructArm() 
{
    innerMuscleRadius = outerBoneRadius;
    outerMuscleRadius = innerMuscleRadius + 25 * mm;
    innerGrasaRadius  = outerMuscleRadius;
    outerGrasaRadius  = innerGrasaRadius + 5 * mm;
    innerSkinRadius   = outerGrasaRadius;
    outerSkinRadius   = innerSkinRadius + 1.5 * mm;

    if (isArmDivided) {ConstructArmDivided();} else 
    {    
        solidBone = new G4Tubs("Bone", innerBoneRadius, outerBoneRadius, boneHeight/2, 0.0, 360.0*deg);
    
        if (isOsBone) {ConstructOsteoporoticBone();} else
        {
            logicBone = new G4LogicalVolume(solidBone, Bone, "LogicBone");
            physBone = new G4PVPlacement(targetRotation, targetPosition, logicBone, "physBone", logicWorld, false, 0, true);
        }
    }

    solidMuscle = new G4Tubs("Muscle",  innerMuscleRadius, outerMuscleRadius, boneHeight/2, 0.0, 360.0*deg);
    solidGrasa  = new G4Tubs("Grasa",   innerGrasaRadius, outerGrasaRadius,   boneHeight/2, 0.0, 360.0*deg);
    solidSkin   = new G4Tubs("Skin",    innerSkinRadius, outerSkinRadius,     boneHeight/2, 0.0, 360.0*deg);

    logicMuscle = new G4LogicalVolume(solidMuscle, Muscle, "LogicMuscle");
    logicGrasa = new G4LogicalVolume(solidGrasa, Fat, "LogicGrasa");
    logicSkin = new G4LogicalVolume(solidSkin, Skin, "LogicSkin");

    physMuscle = new G4PVPlacement(targetRotation, targetPosition, logicMuscle, "physMuscle", logicWorld, false, 0, true);
    physGrasa = new G4PVPlacement(targetRotation, targetPosition, logicGrasa, "physGrasa", logicWorld, false, 0, true);
    physSkin = new G4PVPlacement(targetRotation, targetPosition, logicSkin, "physSkin", logicWorld, false, 0, true);
}

void MyDetectorConstruction::ConstructBone() 
{
    solidBone = new G4Tubs("Bone", innerBoneRadius, outerBoneRadius, boneHeight/2, 0.0, 360.0*deg);
    
    if (isOsBone) ConstructOsteoporoticBone(); else 
    {
        logicBone = new G4LogicalVolume(solidBone, Bone, "LogicBone");
        physBone = new G4PVPlacement(targetRotation, targetPosition, logicBone, "physBone", logicWorld, false, 0, true);
    }
}

void MyDetectorConstruction::ConstructOsteoporoticBone() 
{   
    numPores = 20;
    poreRadius = 0.30 * cm;

    pore = new G4Sphere("Pore", 0, poreRadius, 0 * deg, 360 * deg, 0 * deg, 180 * deg);
    porousBone = solidBone;

    regionMinZ = 0; 
    regionMaxZ = boneHeight / 2; 
    regionMinRadius = 0; 
    regionMaxRadius = outerBoneRadius;

    for (int i = 1; i <= numPores; i++)
    {
        r      = G4RandGauss::shoot(0.6, 0.25) * (regionMaxRadius - regionMinRadius);
        theta  = G4UniformRand() * 360.0 * deg;
        z      = (G4UniformRand() * (regionMaxZ - regionMinZ - poreRadius)) + regionMinZ;
      
        x = r * std::cos(theta);
        y = r * std::sin(theta);

        porePosition = G4ThreeVector(x, y, -z);
        porousBone = new G4SubtractionSolid("PorousBone", porousBone, pore, 0, porePosition);
    }

    logicBone = new G4LogicalVolume(porousBone, Bone, "PorousBoneLogical");
    physBone = new G4PVPlacement(targetRotation, targetPosition, logicBone, "physBone", logicWorld, false, 0);
}

void MyDetectorConstruction::ConstructArmDivided()
{
    osteoBone = new G4Tubs("Healty_Bone", innerBoneRadius, outerBoneRadius, boneHeight/4, 0.0, 360.0*deg);
    healthyBone = new G4Tubs("Osteo_Bone",  innerBoneRadius, outerBoneRadius, boneHeight/4, 0.0, 360.0*deg);
    
    osteo_position = G4ThreeVector(0, boneHeight/4, 0);
    logicOs = new G4LogicalVolume(osteoBone, OsBone, "LogicOs");
    physOs  = new G4PVPlacement(targetRotation, osteo_position, logicOs, "physOs", logicWorld, false, 0, true);

    healthy_position = G4ThreeVector(0, -boneHeight/4, 0);
    logicHealthy = new G4LogicalVolume(healthyBone, Bone, "LogiHealthy");
    physHealthy  = new G4PVPlacement(targetRotation, healthy_position, logicHealthy, "physHealthy", logicWorld, false, 0, true);
}

// Load 3D Models ===============================================================================================================================

void MyDetectorConstruction::ConstructThorax()
{
    G4STL stl; 

    G4double thoraxAngle = 0;
    Model3DRotation = new G4RotationMatrix(0*deg, -90*deg, (thoraxAngle+180)*deg);

    Ribcage = stl.Read("/Users/miguelcomett/geant4-v11.2.2_2/Estancia_G4/G4_IFI-Team_2024/miguelcomett/3D_Models/RIBCAGE_Real.stl");
    if (Ribcage) 
    {
        logicRibcage = new G4LogicalVolume(Ribcage, Bone, "Ribcage");
        new G4PVPlacement(Model3DRotation, targetPosition, logicRibcage, "Ribcage", logicWorld, false, 0, true);

        G4cout << "Modelo bone importado exitosamente" << G4endl;
    }
    else {G4cout << "Modelo bone no importado" << G4endl;}

    Lungs = stl.Read("/Users/miguelcomett/geant4-v11.2.2_2/Estancia_G4/G4_IFI-Team_2024/miguelcomett/3D_Models/LUNGS.stl");
    Heart = stl.Read("/Users/miguelcomett/geant4-v11.2.2_2/Estancia_G4/G4_IFI-Team_2024/miguelcomett/3D_Models/HEART.stl");
    if (Lungs && Heart) 
    {
        logicLungs = new G4LogicalVolume(Lungs, Air, "Lungs");
        new G4PVPlacement(Model3DRotation, targetPosition, logicLungs, "Lungs", logicWorld, false, 0, true);
        logicHeart = new G4LogicalVolume(Heart, Muscle, "Heart");
        new G4PVPlacement(Model3DRotation, targetPosition, logicHeart, "Heart", logicWorld, false, 0, true);

        G4cout << "Modelo LUNGS importado exitosamente" << G4endl; G4cout << "Modelo HEART importado exitosamente" << G4endl;
    }
    else {G4cout << "Modelo LUNGS no importado" << G4endl; G4cout << "Modelo HEART no importado" << G4endl;}

    Thorax1 = stl.Read("/Users/miguelcomett/geant4-v11.2.2_2/Estancia_G4/G4_IFI-Team_2024/miguelcomett/3D_Models/TORAX_Real.stl");
    Thorax2 = stl.Read("/Users/miguelcomett/geant4-v11.2.2_2/Estancia_G4/G4_IFI-Team_2024/miguelcomett/3D_Models/TORAX_Real0.stl");
    if (Thorax1 && Thorax2) 
    {
        originMatrix = new G4RotationMatrix(0,0,0);
        subtractedSolid = new G4SubtractionSolid("SoftWithBoneHole", Thorax1, Ribcage, originMatrix, targetPosition); // Resta el volumen "bone" del volumen "tissue"
        subtractedSolid2 = new G4SubtractionSolid("SoftWithBoneAndToraxHole", subtractedSolid, Thorax2, originMatrix, targetPosition); // Resta el volumen "TORAX_Real0" del resultado anterior
        finalSubtractedSolid = new G4LogicalVolume(subtractedSolid2, Fat, "Thorax"); // Crear el volumen lógico del sólido resultante
        new G4PVPlacement(Model3DRotation, G4ThreeVector(0,0,0), finalSubtractedSolid, "Thorax", logicWorld, false, 0, true);

        G4cout << "Modelo tissue con huecos de bone y TORAX_Real0 importado exitosamente" << G4endl;
    }
    else {G4cout << "Error al importar los modelos STL" << G4endl;}
}

void MyDetectorConstruction::DefineMaterials()
{
    G4NistManager * nist = G4NistManager::Instance();

    // Elements ========================================================================================

    // C  = nist -> FindOrBuildElement("C");
    C  = new G4Element("Carbon",     "C",  6,   12.01*g/mole);
    N  = new G4Element("Nitrogen",   "N",  7,   14.01*g/mole);
    O  = new G4Element("Oxygen",     "O",  8,   16.00*g/mole);
    Mg = new G4Element("Magnesium",  "Mg", 12,  24.31*g/mole);
    Ca = new G4Element("Calcium",    "Ca", 20,  40.08*g/mole);
    V  = new G4Element("Vanadium",   "V",  23,  50.94*g/mole);
    Cd = new G4Element("Cadmium",    "Cd", 48, 112.41*g/mole);
    Te = new G4Element("Tellurium",  "Te", 52, 127.60*g/mole);
    W  = new G4Element("Wolframium", "W",  74, 183.84*g/mole);

    Calcium = new G4Material("Calcium", 1.55*g/cm3, 1);
    Calcium -> AddElement(nist -> FindOrBuildElement("Ca"), 1);

    Magnesium = new G4Material("Magnesium", 1.74*g/cm3, 1);
    Magnesium -> AddElement(nist -> FindOrBuildElement("Mg"), 1);

    Aluminum = new G4Material("Aluminum", 2.70*g/cm3, 1);
    Aluminum -> AddElement(nist -> FindOrBuildElement("Al"), 1);

    Silicon = new G4Material("Silicon", 2.33*g/cm3, 1);
    Silicon -> AddElement(nist -> FindOrBuildElement("Si"), 1);

    Wolframium = new G4Material("Wolframium", 19.25*g/cm3, 1);
    Wolframium -> AddElement(nist -> FindOrBuildElement("W"), 1);

    // Compounds =======================================================================================

    worldMaterial = nist -> FindOrBuildMaterial("G4_AIR");
    
    Air = new G4Material("Air", 0.0001*g/cm3, 2);
    Air -> AddElement(N, 0.78);
    Air -> AddElement(O, 0.22);

    SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2); 
    SiO2 -> AddElement(nist -> FindOrBuildElement("Si"), 1);
    SiO2 -> AddElement(nist -> FindOrBuildElement("O"), 2);

    H2O = new G4Material("H2O", 1.0*g/cm3, 2); 
    H2O -> AddElement(nist -> FindOrBuildElement("H"), 2);
    H2O -> AddElement(nist -> FindOrBuildElement("O"), 1);

    // Glass Materials =======================================================================================
    
    CadTel = nist -> FindOrBuildMaterial("G4_CADMIUM_TELLURIDE");

    V2O5 = new G4Material("V2O5", 3.36*g/cm3, 2);
    V2O5 -> AddElement(V, 2);
    V2O5 -> AddElement(O, 5);

    amorphousGlass = new G4Material("AmorphousGlass", 2.5*g/cm3, 2);
    amorphousGlass -> AddElement(nist -> FindOrBuildElement("Si"), 1);
    amorphousGlass -> AddElement(nist -> FindOrBuildElement("O"), 2);

    fractionMass_VO2 = 0.05, fractionMass_SiO2 = 1.0 - fractionMass_VO2;
    vanadiumGlassMix = new G4Material("VanadiumGlassMix", 2.7*g/cm3, 2);
    vanadiumGlassMix -> AddMaterial(V2O5, fractionMass_VO2);
    vanadiumGlassMix -> AddMaterial(amorphousGlass, fractionMass_SiO2);

    // Organic Materials =======================================================================================

    Bone = nist -> FindOrBuildMaterial("G4_B-100_BONE"); 
    compactBone = nist->FindOrBuildMaterial("G4_BONE_COMPACT_ICRU");
    Muscle = nist -> FindOrBuildMaterial("G4_MUSCLE_SKELETAL_ICRP");
    Fat = nist -> FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
    Skin = nist -> FindOrBuildMaterial("G4_SKIN_ICRP");

    TissueMix = new G4Material("TissueMix", 1.036 * g/cm3, 3); 
    TissueMix -> AddMaterial(Muscle, 79.36 * perCent); 
    TissueMix -> AddMaterial(Fat,    15.87 * perCent); 
    TissueMix -> AddMaterial(Skin,   04.77 * perCent);

    OsBone  =  new G4Material("OsteoporoticBone", 0.80 *g/cm3, 8);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_H"),  06.4 * perCent);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_C"),  27.8 * perCent);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_N"),  02.7 * perCent);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_O"),  41.0 * perCent);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_Mg"), 00.2 * perCent);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_P"),  07.0 * perCent);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_S"),  00.2 * perCent);
    OsBone -> AddMaterial(nist -> FindOrBuildMaterial("G4_Ca"), 14.7 * perCent);

    // Aerogel and Material Proporties =================================================================================

    Aerogel = new G4Material("Aerogel", 10.000*g/cm3, 3);
    Aerogel -> AddMaterial(SiO2, 62.5 * perCent);
    Aerogel -> AddMaterial(H2O , 37.4 * perCent);
    Aerogel -> AddElement (C   , 00.1 * perCent);

    G4double PhotonEnergy[2] = {1.239841939 * eV/0.9, 1.239841939 * eV/0.2};
    G4double RindexAerogel[2] = {1.1, 1.1};
    G4double RindexWorld[2] = {1.0, 1.0};

    G4MaterialPropertiesTable * AerogelProperties = new G4MaterialPropertiesTable();
    AerogelProperties -> AddProperty("RINDEX", PhotonEnergy, RindexAerogel, 2);
    Aerogel -> SetMaterialPropertiesTable(AerogelProperties);
    
    G4MaterialPropertiesTable * worldMaterialProperties = new G4MaterialPropertiesTable();
    worldMaterialProperties -> AddProperty("RINDEX", PhotonEnergy, RindexWorld, 2);
    worldMaterial -> SetMaterialPropertiesTable(worldMaterialProperties);
}