#ifndef DetectorAction_hh
#define DetectorAction_hh

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

extern int arguments;

class MySensitiveDetector : public G4VSensitiveDetector
{
    public:

        MySensitiveDetector(G4String);
        ~MySensitiveDetector();
    
    private: 

        virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);
        G4int digits, defaultDecimals, copyNo, Event;
        G4double Wavelength, Energy;
};

#endif