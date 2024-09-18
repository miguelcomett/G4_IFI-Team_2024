#include "3.0_DetectorConstruction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
    fDetectorMessenger = new G4GenericMessenger(this, "/myDetector/", "Detector Construction");
    fDetectorMessenger -> DeclareProperty("nColumns", nColumns, "Number of columns");
    fDetectorMessenger -> DeclareProperty("nRows", nRows, "Number of rows");
    fDetectorMessenger -> DeclareProperty("ThicknessTarget", thicknessTarget, "Thickness of the target");
    // fDetectorMessenger -> SetDefaultUnit("nm");
    // fDetectorMessenger -> DeclareProperty("MaterialTarget", materialTarget, "Material of the target");

    nColumns = 10;
    nRows    = 10;
    thicknessTarget = .00005 * mm; 
    // materialTarget = SiO2;

    DefineMaterials();
}

MyDetectorConstruction::~MyDetectorConstruction(){}

void MyDetectorConstruction::DefineMaterials()
{
    G4NistManager * nist = G4NistManager::Instance();

    N = new G4Element("Nitrogen", "N", 7, 14.01 * g/mole);
    O = new G4Element("Oxygen"  , "O", 8, 16.00 * g/mole);
    V = new G4Element("Vanadium", "V", 23, 50.94 * g/mole);
    C = nist -> FindOrBuildElement("C");
    Cd = new G4Element("Cadmium", "Cd", 48, 112.41 * g/mole);
    Te = new G4Element("Tellurium", "Te", 52, 127.60 * g/mole);


    worldMaterial = nist -> FindOrBuildMaterial("G4_AIR");
    Air = new G4Material("Air", 0.0000000000000000001*g/cm3, 2);
    Air -> AddElement(N, 0.78);
    Air -> AddElement(O, 0.22);

    SiO2 = new G4Material("SiO2", 2.201*g/cm3, 2); 
    SiO2 -> AddElement(nist -> FindOrBuildElement("Si"), 1);
    SiO2 -> AddElement(nist -> FindOrBuildElement("O"), 2);

    H2O = new G4Material("H2O", 1.000*g/cm3, 2); 
    H2O -> AddElement(nist -> FindOrBuildElement("H"), 2);
    H2O -> AddElement(nist -> FindOrBuildElement("O"), 1);

    Wolframium = new G4Material("Wolframium", 19.25 * g/cm3, 1);
    Wolframium -> AddElement(nist -> FindOrBuildElement("W"), 1);

    Aluminum = new G4Material("Aluminum", 2.70 * g/cm3, 1);
    Aluminum -> AddElement(nist -> FindOrBuildElement("Al"), 1);

    Silicon = new G4Material("Silicon", 2.33 * g/cm3, 1);
    Silicon -> AddElement(nist -> FindOrBuildElement("Si"), 1);
    
    Aerogel = new G4Material("Aerogel", 10.000 * g/cm3, 3);
    Aerogel -> AddMaterial(SiO2, 62.5 * perCent);
    Aerogel -> AddMaterial(H2O , 37.4 * perCent);
    Aerogel -> AddElement (C   , 00.1 * perCent);


    V2O5 = new G4Material("V2O5", 3.36 * g/cm3, 2);
    V2O5 -> AddElement(V, 2);
    V2O5 -> AddElement(O, 5);

    G4Material * amorphousGlass = new G4Material("AmorphousGlass", 2.5 * g / cm3, 2);
    amorphousGlass -> AddElement(nist -> FindOrBuildElement("Si"), 1);
    amorphousGlass -> AddElement(nist -> FindOrBuildElement("O"), 2);

    vanadiumGlassMix = new G4Material("VanadiumGlassMix", 2.7 * g / cm3, 2);
    G4double fractionMass_VO2 = 0.05;
    G4double fractionMass_SiO2 = 1.0 - fractionMass_VO2;
    vanadiumGlassMix -> AddMaterial(V2O5, fractionMass_VO2);
    vanadiumGlassMix -> AddMaterial(amorphousGlass, fractionMass_SiO2);

    CadTel = nist -> FindOrBuildMaterial("G4_CADMIUM_TELLURIDE");

    Bone = nist -> FindOrBuildMaterial("G4_B-100_BONE"); 
    Fat = nist -> FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
    Skin = nist->FindOrBuildMaterial("G4_SKIN_ICRP");
    Muscle = nist->FindOrBuildMaterial("G4_MUSCLE_SKELETAL_ICRP");

    G4double PhotonEnergy[2] = {1.239841939*eV/0.9, 1.239841939*eV/0.2};
    G4double RindexAerogel[2] = {1.1, 1.1};
    G4double RindexWorld[2] = {1.0, 1.0};

    G4MaterialPropertiesTable * AerogelProperties = new G4MaterialPropertiesTable();
    AerogelProperties -> AddProperty("RINDEX", PhotonEnergy, RindexAerogel, 2);
    Aerogel -> SetMaterialPropertiesTable(AerogelProperties);
    
    G4MaterialPropertiesTable * worldMaterialProperties = new G4MaterialPropertiesTable();
    worldMaterialProperties -> AddProperty("RINDEX", PhotonEnergy, RindexWorld, 2);
    worldMaterial -> SetMaterialPropertiesTable(worldMaterialProperties);
}

G4VPhysicalVolume * MyDetectorConstruction::Construct()
{
    bool check_Overlaps = false;
    materialTarget = Bone;
    
    G4double xWorld = 0.5*m;
    G4double yWorld = 0.5*m;
    G4double zWorld = 0.5*m;

    solidWorld = new G4Box("SolidWorld", xWorld, yWorld, zWorld);
    logicWorld = new G4LogicalVolume(solidWorld, Air, "LogicalWorld");
    physicalWorld = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), logicWorld, "PhysicalWorld", 0, false, 0, true);

    solidRadiator = new G4Box("solidRadiator", 0.4*m, 0.4*m, thicknessTarget/2);
    logicRadiator = new G4LogicalVolume(solidRadiator, materialTarget, "logicalRadiator");
    physicalRadiator = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.25*m), logicRadiator, "PhysicalRadiator", logicWorld, false, 0, true);
    // -0.48
    fScoringVolume = logicRadiator;

    solidDetector = new G4Box("solidDetector", xWorld/nRows, yWorld/nColumns, 0.01*m);
    logicDetector = new G4LogicalVolume(solidDetector, worldMaterial, "logicalDetector");

    for(G4int i = 0; i < nRows; i++){
        
        for (G4int j = 0; j < nColumns; j++){
            
            physicalDetector 
            = new G4PVPlacement(0, G4ThreeVector(-0.5*m + (i+0.5)*m/nRows, -0.5*m + (j+0.5)*m/nColumns, 0.49*m), 
            logicDetector, "physicalDetector", logicWorld, false, j + i*nColumns, check_Overlaps);
        }
    }

    return physicalWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector * sensitiveDetector = new MySensitiveDetector("sensitiveDetector");
    logicDetector -> SetSensitiveDetector(sensitiveDetector);
}