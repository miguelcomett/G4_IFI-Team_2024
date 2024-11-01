#ifndef DetectorConstruction_hh
#define DetectorConstruction_hh

#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4GenericMessenger.hh"
#include "G4UnionSolid.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4RandomTools.hh"
#include "G4RunManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4VSolid.hh"
#include "G4Sphere.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SubtractionSolid.hh"

#include "3.1_DetectorAction.hh"
#include "3.2_Geometry3D.hh"
#include "3.3_GeometryReader.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{   
    public:

        MyDetectorConstruction();
        ~MyDetectorConstruction() override;

        void DefineMaterials();
        void ConstructSDandField() override;
        G4VPhysicalVolume * Construct() override;

        G4LogicalVolume * GetScoringVolume() const {return ScoringVolume;}
        G4Material * GetMaterial() const {return materialTarget;}
	    G4double GetThickness() const {return target_Thickness;}

        G4bool  isArm, isHealthyBone, isOsteoBone, isPlacas, isBoneDivided, is3DModel, isTarget, 
                isHeart, isLungs, isRibcage, isFiller, isThorax,
                checkOverlaps;
    
    private:

        void ConstructTarget();
        void ConstructHealthyBone();
        void ConstructOsteoporoticBone();
        void ConstructArm();
        void ConstructTissue();
        void ConstructBoneDivided();
        void ConstructThorax();

        G4GenericMessenger * DetectorMessenger;

        G4int DetColumnNum, DetRowNum, numPores; 
        G4double target_Thickness, innerBoneRadius, outerBoneRadius, boneHeight, poreRadius, xWorld, yWorld, zWorld, 
                 regionMinZ, regionMaxZ, regionMinRadius, regionMaxRadius, r, theta, z, x, y,
                 innerMuscleRadius, outerMuscleRadius, innerGrasaRadius, outerGrasaRadius, innerSkinRadius, outerSkinRadius,
                 fractionMass_VO2, fractionMass_SiO2, fTargetAngle;

        G4Box    * solidWorld, * solidDetector, * solidRadiator;
        G4Tubs   * solidBone, * solidMuscle, * solidGrasa, * solidSkin, * solidBone2, * osteoBone, * healthyBone; 
        G4Sphere * pore;  
        G4VSolid * porousBone; 

        G4LogicalVolume   * logicWorld, * logicRadiator, * logicDetector, * logicHealthyBone, * logicOsteoBone, * logicMuscle, 
                          * logicGrasa, * logicSkin, * logicOs, * logicHealthy, * ScoringVolume,
                          * logicLungs, * logicHeart, * finalSubtractedSolid0, * logicRibcage, * innerVolume;
        G4VPhysicalVolume * physicalWorld, * physicalRadiator, * physicalDetector, * physBone, * physArm, 
                          * physMuscle, * physGrasa, * physSkin, * physOs, * physHealthy;
                        
        G4ThreeVector targetPosition, DetectorPosition, porePosition, osteo_position, healthy_position, Radiator_Position;
        G4RotationMatrix * targetRotation, * Model3DRotation, * originMatrix; 

        G4Element  * C, * Al, * N, * O, * Ca, * Mg, * V, * Cd, * Te, * W;
        G4Material * SiO2, * H2O, * Aerogel, * worldMaterial, * Calcium, * Magnesium, * Aluminum, * Air, * Silicon, * materialTarget, 
                   * CadTel, * vanadiumGlassMix, * amorphousGlass, * Wolframium, * V2O5, 
                   * Fat, * Skin, * Muscle, * Bone, * OsBone, * compactBone, * TissueMix;
        
        STLGeometryReader * stlReader;
        G4TessellatedSolid * Ribcage, * Lungs, * Heart;
        G4VSolid * Thorax1, * Thorax2;
        G4SubtractionSolid * subtractedSolid0, * subtractedSolid1, * subtractedSolid2, * subtractedSolid3, * subtractedSolid4;
};

#endif 