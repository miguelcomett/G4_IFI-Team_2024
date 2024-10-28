#ifndef G4_PCM_PRIMARY_GENERATOR_ACTION_H
#define G4_PCM_PRIMARY_GENERATOR_ACTION_H 1


#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleGun.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"

#include <vector>
#include <fstream>

namespace G4_PCM
{
	class PrimaryGeneratorMessenger; // Forward declaration

	class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
	{
	public:
		PrimaryGeneratorAction();
		~PrimaryGeneratorAction();

		virtual void GeneratePrimaries(G4Event*);
		void SetGunZpos(G4double zpos);
		void SetGunAngle(G4double angle); 
		void SetGunMode(G4int mode); 
		void ReadSpectrumFromFile(const std::string& filename, std::vector<G4double>& xx, std::vector<G4double>& yy, G4int& fNPoints);
		G4double InverseCumul();

		G4ParticleGun* fParticleGun;
	private:
		G4double fPgun = -5. * cm; // Valor predeterminado
		G4double fGunAngle = 45; //Predeterminado
		G4double energy; 
		G4int fGunMode; 
		G4double radius; 
		
		G4UIcmdWithADoubleAndUnit *fPgunCmd, *fPgunAngleCmd;
		PrimaryGeneratorMessenger* fMessenger; // Pointer to the messenger
		
		//Spectrum
		G4int                  fNPoints = 0; //nb of points
		std::vector<G4double>  fX;           //abscisses X
		std::vector<G4double>  fY;           //values of Y(X)
		std::vector<G4double>  fSlp;         //slopes
		std::vector<G4double>  fYC;          //cumulative function of Y
		G4double               fYmax = 0.;   //max(Y)
		void InitFunction(); 
	};
}


#endif 
