#ifndef PrimaryGenerator_hh
#define PrimaryGenerator_hh

#include <iomanip>
#include <vector>
#include <fstream>

#include "Randomize.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"
#include "G4PhysicalConstants.hh"

#include "3.0_DetectorConstruction.hh"
#include "6.1_Run.hh"
#include "5.1_GeneratorMessenger.hh"

class PrimaryGeneratorMessenger; 
class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
    public:

        MyPrimaryGenerator();
        ~ MyPrimaryGenerator();

        virtual void GeneratePrimaries(G4Event *);
        void SetGunZpos(G4double zpos);
        void SetGunRadius(G4double radius); 
        void SetGunAngle(G4double angle); 
        void SetGunMode(G4int mode); 
	
        G4ParticleGun * GetParticleGun() const {return particleGun;}
        
        //Real spectrum feature
        void ReadSpectrumFromFile(const std::string & filename, std::vector<G4double> & xx, std::vector<G4double> & yy, G4int & fNPoints);
        G4double InverseCumul();
    
    private:

        G4ParticleGun * particleGun;        
        PrimaryGeneratorMessenger * GeneratorMessenger;

        G4String particleName;
        G4ParticleTable * particleTable;
        G4ParticleDefinition * particle;
        
        G4bool fullAngle;
        G4double radius, theta, phi, angle, angleInRadians, angleInCarts, x0, y0, z0;
        const G4double pi = 3.14159265358979323846;
        
        // Real spectrum
        void InitFunction(); 
        
        G4double energy;
        
        G4String spectrumFileName; 	       
        G4int                  fNPoints = 0; //nb of points
        std::vector<G4double>  fX;           //abscisses X
        std::vector<G4double>  fY;           //values of Y(X)
        std::vector<G4double>  fSlp;         //slopes
        std::vector<G4double>  fYC;          //cumulative function of Y
        G4double               fYmax = 0.;   //max(Y)
        
        // Messangers   
        G4int fGunMode;
        G4double fPgun, fGunAngle, fGunRadius, realEnergy;
};

#endif