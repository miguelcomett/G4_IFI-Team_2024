#ifndef DetectorConstruction_hh
#define DetectorConstruction_hh

#include <vector> 
#include <string>

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

        G4LogicalVolume * GetScoringVolume() const {return scoringVolume_0;}

        std::vector<G4LogicalVolume*> scoringVolumes;
        std::vector<G4LogicalVolume*> GetAllScoringVolumes() const { return scoringVolumes; }

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

        G4int numPores;
        G4int DetColumnNum = 10, DetRowNum = 10; 
        
        G4double innerBoneRadius, outerBoneRadius, boneHeight, poreRadius, xWorld, yWorld, zWorld, 
                 regionMinZ, regionMaxZ, regionMinRadius, regionMaxRadius, r, theta, z, x, y,
                 innerMuscleRadius, outerMuscleRadius, innerGrasaRadius, outerGrasaRadius, innerSkinRadius, outerSkinRadius,
                 fractionMass_VO2, fractionMass_SiO2, fTargetAngle;
        G4double thoraxAngle = 0.0, target_Thickness = 40 * mm;

        G4Box    * solidWorld, * solidDetector, * solidRadiator;
        G4Tubs   * solidBone, * solidMuscle, * solidGrasa, * solidSkin, * solidBone2, * osteoBone, * healthyBone; 
        G4Sphere * pore;  
        G4VSolid * porousBone; 

        G4LogicalVolume   * logicWorld, * logicRadiator, * logicDetector, * logicHealthyBone, * logicOsteoBone, * logicMuscle, 
                          * logicGrasa, * logicSkin, * logicOs, * logicHealthy, 
                          * logicLungs, * logicHeart, * logicThorax, * logicRibcage, * logicFiller,
                          * scoringVolume_0, * scoringVolume_1, * scoringVolume_2, * scoringVolume_3, * scoringVolume_4, * scoringVolume_5; 
        G4VPhysicalVolume * physicalWorld, * physicalRadiator, * physicalDetector, * physBone, * physArm, 
                          * physMuscle, * physGrasa, * physSkin, * physOs, * physHealthy;
                        
        G4ThreeVector armPosition, DetectorPosition, porePosition, osteo_position, healthy_position, Radiator_Position;
        G4RotationMatrix * armRotation, * Model3DRotation, * originMatrix; 

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