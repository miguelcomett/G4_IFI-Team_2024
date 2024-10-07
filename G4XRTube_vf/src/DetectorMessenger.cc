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
/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction *Det)
    : G4UImessenger(),
      fDetector(Det), fXRayTubeDir(0), fDetDir(0),
      fTargMatCmd(0), fFilterMatCmd(0),
      fAnodeAngleCmd(0), fFilterThicknessCmd(0)
{
  fXRayTubeDir = new G4UIdirectory("/XRtube/");
  fXRayTubeDir->SetGuidance("commands specific to this example");

  G4bool broadcast = false;
  fDetDir = new G4UIdirectory("/XRtube/det/", broadcast);
  fDetDir->SetGuidance("detector construction commands");

  fTargMatCmd = new G4UIcmdWithAString("/XRtube/det/setTargetMaterial", this);
  fTargMatCmd->SetGuidance("Select material of the target");
  fTargMatCmd->SetParameterName("G4_Material", false);
  fTargMatCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fFilterMatCmd = new G4UIcmdWithAString("/XRtube/det/setFilterMaterial", this);
  fFilterMatCmd->SetGuidance("Select material of the filter");
  fFilterMatCmd->SetParameterName("G4_Material", false);
  fFilterMatCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fAnodeAngleCmd =
      new G4UIcmdWithADoubleAndUnit("/XRtube/det/setAnodeAngle", this);
  fAnodeAngleCmd->SetGuidance("Set the x-ray tube anode angle.");
  fAnodeAngleCmd->SetUnitCategory("Angle");
  fAnodeAngleCmd->SetParameterName("Anode Angle", false);
  fAnodeAngleCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fFilterThicknessCmd =
      new G4UIcmdWithADoubleAndUnit("/XRtube/det/setFilterThickness", this);
  fFilterThicknessCmd->SetGuidance("Set the the filter thickness.");
  fFilterThicknessCmd->SetUnitCategory("Length");
  fFilterThicknessCmd->SetParameterName("Filter Thickness", false);
  fFilterThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......fFilterThicknessCmd

DetectorMessenger::~DetectorMessenger()
{
  delete fTargMatCmd;
  delete fAnodeAngleCmd;
  delete fFilterThicknessCmd;
  delete fFilterMatCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
  if (command == fTargMatCmd)
  {
    fDetector->SetAodeMaterial(newValue);
  }

  if (command == fFilterMatCmd)
  {
    fDetector->SetFilterMaterial(newValue);
  }

  if (command == fAnodeAngleCmd)
  {
    fDetector->SetAnodeAngle(
        fAnodeAngleCmd->GetNewDoubleValue(newValue));
  }

  if (command == fFilterThicknessCmd)
  {
    fDetector->SetFilterThickness(
        fFilterThicknessCmd->GetNewDoubleValue(newValue));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
