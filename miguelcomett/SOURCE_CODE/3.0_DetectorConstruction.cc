#include "3.0_DetectorConstruction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
    DefineMaterials();

    fDetectorMessenger = new G4GenericMessenger(this, "/myDetector/", "Detector Construction");
    fDetectorMessenger -> DeclareProperty("nColumns", DetColumnNum, "Number of columns");
    fDetectorMessenger -> DeclareProperty("nRows", DetRowNum, "Number of rows");
    fDetectorMessenger -> DeclareProperty("ThicknessTarget", target_Thickness, "Thickness of the target");

    DetColumnNum = 10, DetRowNum = 10;
    
    boneHeight = 60 * mm;
    innerBoneRadius = 0.0;
    outerBoneRadius = 22.5 * mm;
    targetRotation = new G4RotationMatrix(0, 90*deg, 0);
    
    isArm = true;
    isBone = false;
    isOsBone = false;
    isArmDivided = true;
}

MyDetectorConstruction::~MyDetectorConstruction(){}


G4VPhysicalVolume * MyDetectorConstruction::Construct()
{
    xWorld = 0.5*m, yWorld = 0.5*m, zWorld = 0.5*m;
    solidWorld = new G4Box("SolidWorld", xWorld, yWorld, zWorld);
    logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "LogicalWorld");
    physicalWorld = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), logicWorld, "PhysicalWorld", 0, false, 0, true);

    if (isArm) ConstructArm(); else if (isBone) ConstructBone(); else ConstructTarget();

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


void MyDetectorConstruction::ConstructTarget()
{
    materialTarget = Wolframium;
    target_Thickness = 10 * mm; 

    Radiator_Position = G4ThreeVector(0.0, 0.0, 0.25*m);

    solidRadiator = new G4Box("solidRadiator", 0.05*m, 0.05*m, target_Thickness/2);
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
            physBone = new G4PVPlacement(targetRotation, targetPos, logicBone, "physBone", logicWorld, false, 0, true);
        }
    }

    solidMuscle = new G4Tubs("Muscle",  innerMuscleRadius, outerMuscleRadius, boneHeight/2, 0.0, 360.0*deg);
    solidGrasa  = new G4Tubs("Grasa",   innerGrasaRadius, outerGrasaRadius,   boneHeight/2, 0.0, 360.0*deg);
    solidSkin   = new G4Tubs("Skin",    innerSkinRadius, outerSkinRadius,     boneHeight/2, 0.0, 360.0*deg);

    logicMuscle = new G4LogicalVolume(solidMuscle, Muscle, "LogicMuscle");
    logicGrasa = new G4LogicalVolume(solidGrasa, Fat, "LogicGrasa");
    logicSkin = new G4LogicalVolume(solidSkin, Skin, "LogicSkin");

    physMuscle = new G4PVPlacement(targetRotation, targetPos, logicMuscle, "physMuscle", logicWorld, false, 0, true);
    physGrasa = new G4PVPlacement(targetRotation, targetPos, logicGrasa, "physGrasa", logicWorld, false, 0, true);
    physSkin = new G4PVPlacement(targetRotation, targetPos, logicSkin, "physSkin", logicWorld, false, 0, true);
}

void MyDetectorConstruction::ConstructBone() 
{
    solidBone = new G4Tubs("Bone", innerBoneRadius, outerBoneRadius, boneHeight/2, 0.0, 360.0*deg);
    
    if (isOsBone) ConstructOsteoporoticBone(); else 
    {
        logicBone = new G4LogicalVolume(solidBone, Bone, "LogicBone");
        physBone = new G4PVPlacement(targetRotation, targetPos, logicBone, "physBone", logicWorld, false, 0, true);
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
    physBone = new G4PVPlacement(targetRotation, targetPos, logicBone, "physBone", logicWorld, false, 0);
}

void MyDetectorConstruction::ConstructArmDivided()
{
    osteoBone = new G4Tubs("Healty_Bone", innerBoneRadius, outerBoneRadius, boneHeight/4, 0.0, 360.0*deg);
    healthyBone = new G4Tubs("Osteo_Bone",  innerBoneRadius, outerBoneRadius, boneHeight/4, 0.0, 360.0*deg);
    
    position_1 = G4ThreeVector(0, boneHeight/2, 0);
    logicOs = new G4LogicalVolume(osteoBone, OsBone, "LogicOs");
    physOs  = new G4PVPlacement(targetRotation, position_1, logicOs, "physOs", logicWorld, false, 0, true);

    pos_2 = G4ThreeVector(0, -boneHeight/2, 0);
    logicHealthy = new G4LogicalVolume(healthyBone, Bone, "LogiHealthy");
    physHealthy  = new G4PVPlacement(targetRotation, pos_2, logicHealthy, "physHealthy", logicWorld, false, 0, true);
}


void MyDetectorConstruction::DefineMaterials()
{
    G4NistManager * nist = G4NistManager::Instance();

    // Elements ========================================================================================

    C = nist -> FindOrBuildElement("C");
    N = new G4Element("Nitrogen", "N", 7, 14.01*g/mole);
    O = new G4Element("Oxygen"  , "O", 8, 16.00*g/mole);
    V = new G4Element("Vanadium", "V", 23, 50.94*g/mole);
    Cd = new G4Element("Cadmium", "Cd", 48, 112.41*g/mole);
    Te = new G4Element("Tellurium", "Te", 52, 127.60*g/mole);
    W = new G4Element("Wolframium", "W", 74, 183.84*g/mole);

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
    Fat = nist -> FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
    Skin = nist -> FindOrBuildMaterial("G4_SKIN_ICRP");
    Muscle = nist -> FindOrBuildMaterial("G4_MUSCLE_SKELETAL_ICRP");

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