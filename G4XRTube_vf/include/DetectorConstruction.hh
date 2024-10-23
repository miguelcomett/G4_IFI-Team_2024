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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Para.hh"
#include "G4RotationMatrix.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"

#include "G4STL.hh"
#include "STLGeometryReader.hh"

class G4LogicalVolume;
class G4Material;
class DetectorMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

public:
  virtual G4VPhysicalVolume *Construct();
  virtual void ConstructSDandField();

  void SetAodeMaterial(G4String);
  void SetFilterMaterial(G4String);
  void SetAnodeAngle(G4double value);
  void SetFilterThickness(G4double value);
  G4Material *GetFilterMaterial();
  G4Material *GetTargetMaterial();
  G4double GetAnodeAngle();
  G4double GetFilterThickness();
  void ConstructArm(); 
  void ConstructRealBone(); 
  
  void ConstructBONE3D();
  void ConstructSOFT3D();
  void ConstructSOFT3Dbool();

private:
// Instancia para la clase STLGeometryReader
  G4TessellatedSolid* stlSolid;
  G4TessellatedSolid* stlSolid2;
  G4TessellatedSolid* stlSolid3;
  STLGeometryReader* stlReader;
  G4Material *material3D, *material3Dsoft;
  G4LogicalVolume *logicSTL, *logicSTL2, *logicSTL22; 
  
  //Construction instances
  G4VPhysicalVolume *worldPV;
  G4double fAnodeAngle, fFilterThickness, fTargetAngle;
  G4Material *fWorldMater;
  G4Material *fAnodeMaterial;
  G4Material *fFilterMaterial;
  G4Material *vacuum, *lead, *air, *pyrex, *CdTe, *NaI;
  G4LogicalVolume *detectorLV, *anodeLV, *filterLV, *worldLV;
  //Mats brazo
  G4Tubs *solidMuscle, *solidGrasa, *solidSkin, *solidTrabecular, *solidCortical; 
  G4Material *skin, *grasa, *muscle, *bone, *trabecularBone; 
  
  G4double outerBoneRadius, innerBoneRadius, fTargetThickness, targetX, targetY, targetZ; 
  G4RotationMatrix *targetRotation; 

  DetectorMessenger *fDetectorMessenger;
  
  //OPTIONS
  G4bool isArm, isTorax; 

private:
  G4VPhysicalVolume *ConstructVolumes();
  void DefineMaterials();
  G4bool fCheckOverlaps; // option to activate checking of volumes overlaps
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
