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

#include "3.1_DetectorAction.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
        void ConstructBone();
        void ConstructOsBone();
        void ConstructArm();
        void ConstructTissue();

    public:
        MyDetectorConstruction();
        ~MyDetectorConstruction();

        G4LogicalVolume * GetScoringVolume() const {return fScoringVolume;}

        virtual G4VPhysicalVolume * Construct();

        G4Material * GetMaterial() const {return materialTarget;}
	    G4double GetThickness() const {return thicknessTarget;}
    
    private:
        G4Box * solidWorld, * solidDetector, * solidRadiator;
        G4Tubs * solidBone, * solidMuscle, * solidGrasa, * solidSkin, * solidBone2, *prueba; 
        G4Sphere * pore;  
        G4VSolid * porousBone; 
        G4RotationMatrix * targetRotation; 
        G4LogicalVolume * logicWorld, * logicRadiator, * logicDetector, 
                        * fScoringVolume, * fScoringVolume1, * fScoringVolume2, * fScoringVolume3,  
                        * logicBone, * logicMuscle, * logicGrasa, * logicSkin, *pruebaLog;
        G4VPhysicalVolume * physicalWorld, * physicalRadiator, * physicalDetector, * physBone, * physArm, 
                          * physMuscle, * physGrasa, * physSkin, *pruebaPhys;
        G4ThreeVector targetPos;

        virtual void ConstructSDandField();
        
        void DefineMaterials();

        G4Material * SiO2, * H2O, * Aerogel, * worldMaterial, * Aluminum, * Air, 
                   * Silicon, * materialTarget, * Wolframium, * V2O5, * Bone, * OsBone, * compactBone, * E_PbWO4, 
                   * CadTel, * vanadiumGlassMix, * amorphousGlass, * Fat, * Skin, * Muscle;
                   
        G4Element * C, * Al, * N, * O, * V, * Cd, * Te;

        G4bool isArm, isBone, isOsBone, isPlacas;

        G4int nColumns, nRows; 
        G4GenericMessenger * fDetectorMessenger;  

        G4double thicknessTarget, outerBoneRadius, fTargetThickness;
};

#endif 