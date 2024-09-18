#ifndef DetectorAction_hh
#define DetectorAction_hh

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

class MySensitiveDetector : public G4VSensitiveDetector
{
    public:
        MySensitiveDetector(G4String);
        ~MySensitiveDetector();
    
    private: 
        virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
};

#endif