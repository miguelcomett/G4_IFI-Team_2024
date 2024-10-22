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
// Authors: Susanna Guatelli and Francesco Romano
// susanna@uow.edu.au, francesco.romano@ct.infn.it
//
//  code based on the basic example B2
//
#include "SensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

SensitiveDetector::SensitiveDetector(const G4String &name,
                                     const G4String &hitsCollectionName)
    : G4VSensitiveDetector(name),
      fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
  // analysis = analysis_manager;
}

SensitiveDetector::~SensitiveDetector()
{
}

void SensitiveDetector::Initialize(G4HCofThisEvent *hce)
{
  // Create hits collection

  fHitsCollection = new SensitiveDetectorHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce

  G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool SensitiveDetector::ProcessHits(G4Step *aStep,
                                      G4TouchableHistory *)
{
  G4Track *track = aStep->GetTrack();

  track->SetTrackStatus(fStopAndKill);

  if (track->GetDynamicParticle()->GetDefinition()->GetParticleName() == "gamma")
  {
    G4ThreeVector fPosition = aStep->GetPreStepPoint()->GetPosition();
    G4double E = aStep->GetPreStepPoint()->GetKineticEnergy() / keV;
    G4double w = aStep->GetPreStepPoint()->GetWeight();
    // G4cout << E << "--------------->" << w << "\n";
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, 0, E);
    man->AddNtupleRow(0);
    //man->FillH1(0, E,w);
    
    man->FillNtupleDColumn(1, 0, fPosition.getX());
    man->FillNtupleDColumn(1, 1, fPosition.getY());
    man->FillNtupleDColumn(1, 2, fPosition.getZ());
    man->AddNtupleRow(1);
  }

  // if (volumeName != "detector")
  //   return false;

  SensitiveDetectorHit *newHit = new SensitiveDetectorHit();

  // newHit->SetEdep(edep);

  fHitsCollection->insert(newHit);

  return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *)
{

  // Initialisation of total energy deposition per event to zero
  G4double totalEdepInOneEvent = 0;

  G4int NbHits = fHitsCollection->entries();
  // G4cout << "number of hits " <<NbHits << G4endl;

  // for (G4int i = 0; i < NbHits; i++)
  // {
  //   G4double edep = (*fHitsCollection)[i]->GetEdep();
  //   totalEdepInOneEvent = totalEdepInOneEvent + edep;
  // }
  // if (totalEdepInOneEvent != 0)
  // {
  //   G4AnalysisManager *man = G4AnalysisManager::Instance();

  //   man->FillNtupleDColumn(0, 0, totalEdepInOneEvent);
  //   man->AddNtupleRow(0);
  //   man->FillH1(0, totalEdepInOneEvent);
  // }
  // G4cout << "############## totalEdepInOneEvent ##############" << totalEdepInOneEvent / keV << G4endl;
}
