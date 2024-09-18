#include "DetectorConstruction.hh"

namespace G4_PCM
{
    DetectorConstruction::DetectorConstruction()
        : fTargetThickness(50 * mm), // Valor predeterminado
        fMessenger(new DetectorConstructionMessenger(this)) // Crear el mensajero
    {
    	DefineMaterials();
    	
    	// Arquitecture of target just -SELECT ONE-
    	isArm = true; 
    	isSingleBone = false; 
    	// Type of bone - SELECT ONE - 	 
	isNormalBone = false; //just solid bone
    	isRealisticBone = true; //solidTrabecular and solidCortical
    	//Want your bone to have osteo? 
    	isOsBone = true; 
	//Filter to eliminate noise    	  
    	isFiltered = false; // Create a wall that just let the circle
    	
    	//Radio total del hueso
    	if (isRealisticBone)
    	{
    		outerBoneRadius = 2.25 * cm; 
    	}
    	else //Hueso normal prueba
    	{
	    	outerBoneRadius = 1.5 * cm;
    	}
    	innerBoneRadius = 0.0;
    	
    	targetRotation = new G4RotationMatrix(0, 90 * deg, 0); // 0, 90 * deg, 0
    	
    	//Detector SIze
        detectorSizeXY = 20 * cm;
        detectorSizeZ = 5 * cm;
        
        // Filter parameters
        filterThick = 1.0*cm / 2; 
        	
    }

    DetectorConstruction::~DetectorConstruction()
    {
        delete fMessenger; // Eliminar el mensajero
    }
    
    void DetectorConstruction::DefineMaterials()
    {
    	// Get nist material manager
        G4NistManager* nist = G4NistManager::Instance();
//G4_B-100_BONE
        // Define el material para el objetivo
        bone = nist->FindOrBuildMaterial("G4_BONE_CORTICAL_ICRP");
        muscle = nist->FindOrBuildMaterial("G4_MUSCLE_SKELETAL_ICRP");
	skin = nist->FindOrBuildMaterial("G4_SKIN_ICRP");
	grasa = nist->FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
        //target = nist->FindOrBuildMaterial("G4_W");
        vacuum = nist->FindOrBuildMaterial("G4_AIR");
        
        // Configure Lead Tungstate for crystals
        E_PbWO4 = new G4Material("E_PbWO4", 8.28 * g / cm3, 3);
        E_PbWO4->AddElement(nist->FindOrBuildElement("Pb"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("W"), 1);
        E_PbWO4->AddElement(nist->FindOrBuildElement("O"), 4);
        
        // Configure material for oesteoporotic bone
        H = nist->FindOrBuildMaterial("G4_H"); 
        C = nist->FindOrBuildMaterial("G4_C"); 
        N = nist->FindOrBuildMaterial("G4_N"); 
        O = nist->FindOrBuildMaterial("G4_O"); 
        Mg = nist->FindOrBuildMaterial("G4_Mg"); 
        P = nist->FindOrBuildMaterial("G4_P"); 
        S = nist->FindOrBuildMaterial("G4_S"); 
        Ca = nist->FindOrBuildMaterial("G4_Ca"); 
        OsBone = new G4Material("OsteoporoticBone", 1.3*g/cm3, 8); //Less d
        OsBone -> AddMaterial(H, 6.4*perCent); 
        OsBone -> AddMaterial(C, 27.8*perCent); 
        OsBone -> AddMaterial(N, 2.7*perCent); 
        OsBone -> AddMaterial(O, 41*perCent); 
        OsBone -> AddMaterial(Mg, 0.2*perCent); 
        OsBone -> AddMaterial(P, 7*perCent); 
        OsBone -> AddMaterial(S, 0.2*perCent); 
        OsBone -> AddMaterial(Ca, 14.7*perCent); 
        
        //Configure mats for W filter
        W = nist->FindOrBuildMaterial("G4_W"); 
        
        //Materials for realistic normal bone
        trabecularBone = nist->FindOrBuildMaterial("G4_B-100_BONE");
        //Realiscti Osteporosis
        F = nist -> FindOrBuildMaterial("G4_F"); 
        RealOsBone = new G4Material("RealOsteoporoticBone", 1.3*g/cm3, 6);
        RealOsBone -> AddMaterial(H, 6.54709*perCent); 
        RealOsBone -> AddMaterial(C, 53.6944*perCent); 
        RealOsBone -> AddMaterial(N, 2.15*perCent); 
        RealOsBone -> AddMaterial(O, 3.2085*perCent); 
        RealOsBone -> AddMaterial(F, 16.7411*perCent); 
        RealOsBone -> AddMaterial(Ca, 17.6589*perCent);  
        
    }
    // ---------- 3 KIND OF BONES CONSTRUCTION -----------------------------
    //----------- OsBone ------- RealBone -------- NormalBone --------------)
    
     // -----------------------------------------------------------------------
     // --------------------------- CONSTRUCT THE BONE WITH PORES
    void DetectorConstruction::ConstructOsBone()
    {
    	//AGREGAR CASO REAL
    	G4double startPhi = 0.*deg;
	G4double deltaPhi = 360.*deg;
	G4double startTheta = 0.*deg;
	G4double deltaTheta = 180.*deg;  // Esfera completa
		// Define los poros como esferas pequeñas
	G4double poreRadius = 100*um; //Ideal 100um
	pore = new G4Sphere("Pore", 0, poreRadius, startPhi, deltaPhi, startTheta, 	deltaTheta);
		// Número de poros que deseas simular
	int numPores = 300; //Quantity 300
		//Solido
	
	if (isNormalBone)
	{
		porousBone = solidBone; 
	}
	if (isRealisticBone)
	{
		porousBone = solidTrabecular; 
	}
	for (int i = 1; i < numPores; i++) {
			// Generar coordenadas aleatorias dentro del cilindro
		if (isNormalBone)
		{
			r = G4UniformRand() * outerBoneRadius; // Distancia radial aleatoria				
		}
		else
		{
			r = G4UniformRand() * (outerBoneRadius - 0.25); 
		}
		G4double theta = G4UniformRand() * 360.0 * deg; // Ángulo aleatorio
		G4double z = G4UniformRand() * fTargetThickness - fTargetThickness/2; // Altura aleatoria en el cilindro
			// Convertir coordenadas cilíndricas a cartesianas
		G4double x = r * std::cos(theta);
		G4double y = r * std::sin(theta);

			// Definir la posición del poro
		G4ThreeVector porePosition = G4ThreeVector(x, y, z);

			// Crear y restar el poro del volumen del hueso
		porousBone = new G4SubtractionSolid("PorousBone", porousBone, pore, 0, porePosition);
		}
		
	if (isNormalBone)
	{
		logicBone = new G4LogicalVolume(porousBone,OsBone,"PorousBoneLogical");
		physBone = new G4PVPlacement(targetRotation, targetPos, logicBone, "physBone", logicWorld, false, 0);  		
	}
	else 
	{
		logicTrabecular = new G4LogicalVolume(porousBone,RealOsBone,"PorousBoneLogical");
		physTrabecular = new G4PVPlacement(targetRotation, targetPos, logicTrabecular, "physTrabecular", logicWorld, false, 0);  
	}

    }
    // ----------------------------------- REALISTIC BONE -----------------------
    // --------------------------------------------------------------------
    void DetectorConstruction::ConstructRealBone()
    {
    	G4double outerTrabecularRadius = outerBoneRadius - 0.25*cm; // = 2cm
    	G4double innerCorticalBone = outerTrabecularRadius; 
    	G4double outerCorticalBone = outerBoneRadius;  // == 2.25
    	
    	//Tubs
    	solidTrabecular = new G4Tubs("TrabecularBone", innerBoneRadius, outerTrabecularRadius, fTargetThickness / 2.0, 0.0, 360.0 *deg); 
    	solidCortical = new G4Tubs("CorticalBone", innerCorticalBone, outerCorticalBone, fTargetThickness/ 2.0, 0.0, 360.0 * deg); 
    	
    	if (isOsBone)
    	{
    		ConstructOsBone(); 
    	}
    	else //Standard real bone
    	{
		logicTrabecular = new G4LogicalVolume(solidTrabecular, trabecularBone, "logicTrabecular"); 
	    	physTrabecular = new G4PVPlacement(targetRotation, targetPos, logicTrabecular, "physTrabecular", logicWorld, false, 0); 
    	}
    	//Logical
    	logicCortical = new G4LogicalVolume(solidCortical, bone, "LogicCortical"); 
    	//Physical Volume
    	physCortical = new G4PVPlacement(targetRotation, targetPos, logicCortical, "physCortical", logicWorld, false, 0); 
    }
    
    // --------------------------------------------------------------------------
    // ---------------------------------------- CONSTRUCT NORMAL BONE
    //Construct Normal Bone
    void DetectorConstruction::ConstructNormalBone()
    {
    	solidBone = new G4Tubs("Bone", innerBoneRadius, outerBoneRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
    	
    	if (isOsBone)
    	{
    		ConstructOsBone(); 
    	}
    	else
    	{
		logicBone = new G4LogicalVolume(solidBone,bone,"LogicBone");
		physBone = new G4PVPlacement(targetRotation, targetPos, logicBone, "physBone", logicWorld, false, 0, true); 

    	}
    
    }
    
    	// -----------------------------------------------------------------------
	// ------------- CREATE ARM --------------------------------
    void DetectorConstruction::ConstructArm()
    {
    	// Crear el antebrazo hueso -> musculo -> grasa -> piel
		
		// MUSCULO
	G4double innerMuscleRadius = outerBoneRadius;
	G4double outerMuscleRadius =  innerMuscleRadius + 2.5 * cm;
		// GRASA
	G4double innerGrasaRadius = outerMuscleRadius;
	G4double outerGrasaRadius =  innerGrasaRadius + 0.5 * cm;
		//PIEL
	G4double innerSkinRadius = outerGrasaRadius;
	G4double outerSkinRadius =  innerSkinRadius + 0.15 * cm;
		
		// Tubs 
	solidMuscle = new G4Tubs("Muscle", innerMuscleRadius, outerMuscleRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
	solidGrasa = new G4Tubs("Grasa", innerGrasaRadius, outerGrasaRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);  
	solidSkin = new G4Tubs("Skin", innerSkinRadius, outerSkinRadius, fTargetThickness / 2.0, 0.0, 360.0 * deg);
	// ------------------NORMAL BONE------------------------
	if (isNormalBone)                                      
	{                                                      
		ConstructNormalBone();                         
	}                                                      
	// ------------------REALISTIC BONE--------------------
	if (isRealisticBone)                                    
	{                                                      
		ConstructRealBone();                           
	}                                                      
	// ----------------------------------------------------
	     	//Logical
	logicMuscle = new G4LogicalVolume(solidMuscle,muscle,"LogicMuscle");
	logicGrasa = new G4LogicalVolume(solidGrasa,grasa,"LogicGrasa");
	logicSkin = new G4LogicalVolume(solidSkin,skin,"LogicSkin");
	     	//PphysVOlume
	     	
	physMuscle = new G4PVPlacement(targetRotation, targetPos, logicMuscle, "physMuscle", logicWorld, false, 0, true); 
	physGrasa = new G4PVPlacement(targetRotation, targetPos, logicGrasa, "physGrasa", logicWorld, false, 0, true); 
	physSkin = new G4PVPlacement(targetRotation, targetPos, logicSkin, "physSkin", logicWorld, false, 0, true); 
	
    }
    
	// -----------------------------------------------------------------------
	// ------------------------------ CREATE SINGLE BONE -------------------------- 
    void DetectorConstruction::ConstructSingleBone()
    {
	if (isNormalBone)
	{
		ConstructNormalBone(); 
	}
	else
	{
		ConstructRealBone(); 
	}
    }
    
    // -----------------------------------------------------------------------------------------
    // ------------------------------------------ FILTER WALL -----------------------------------
    
    void DetectorConstruction::ConstructFilter()
    {	
    	G4Tubs *solidBone2 = new G4Tubs("Bonetest", 0.0, 1.1 * cm, fTargetThickness / 2.0, 0.0, 360.0 * deg); 
    	filterPos = G4ThreeVector(0, 0, -fTargetThickness / 2 - filterThick); 
    	solidFilter = new G4Box("solidFilter", detectorSizeXY, detectorSizeXY, filterThick); 
    	// Restar el cilindro (brazo) de la pared
	G4SubtractionSolid* filterWithHole = new G4SubtractionSolid("FilterWithHole", solidFilter, solidBone2, 0, targetPos);
	logicFilter = new G4LogicalVolume(filterWithHole, W, "LogicFilter"); 
	physFilter = new G4PVPlacement(0, filterPos, logicFilter, "physFilter", logicWorld,false, 0, true); 
    	
    	
    }
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// ------------------ CONSTRUCT GEOM ---------------------------- 
    G4VPhysicalVolume* DetectorConstruction::Construct()
    {
 
        // Define el tamaño del mundo
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
            
        //Define the position and rotation of the tubs no matter isArm or bone
        targetPos = G4ThreeVector(); // 0,0,0
        //targetRotation = new G4RotationMatrix(90, 0, 0);
	
	// type selection
	if(isArm)
	{
		ConstructArm(); 
     	}
     	
     	if(isSingleBone)
     	{
     		ConstructSingleBone(); 
     	}
     	
        G4Box* solidDetector = new G4Box(
            "Detector",
            detectorSizeXY,
            detectorSizeXY,
            detectorSizeZ);

        G4LogicalVolume* logicDetector = new G4LogicalVolume(
            solidDetector,
            E_PbWO4,
            "Detector");

        G4ThreeVector detectorPos = G4ThreeVector(0, 0, 20 * cm);
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

        return physWorld;
    }

    void DetectorConstruction::SetTargetThickness(G4double thickness)
    {
        G4cout << "Setting target thickness to: " << thickness << G4endl;
        if (thickness != fTargetThickness) {
            fTargetThickness = thickness;
            G4cout << "Target thickness changed to: " << fTargetThickness << G4endl;

            // Forzar la actualización de la geometría
            //G4RunManager::GetRunManager()->ReinitializeGeometry();
        }
        else {
            G4cout << "Target thickness unchanged." << G4endl;
        }
    }
}
