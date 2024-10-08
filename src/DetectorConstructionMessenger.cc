#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"

namespace G4_PCM
{
    DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* detector)
        : fDetector(detector)
    {
        fTargetThicknessCmd = new G4UIcmdWithADoubleAndUnit("/det/t", this);
        fTargetThicknessCmd->SetGuidance("Set the target thickness.");
        fTargetThicknessCmd->SetParameterName("thickness", true);
        fTargetThicknessCmd->SetRange("thickness > 0.");
        fTargetThicknessCmd->SetDefaultUnit("mm");

        fTargetAngleCmd = new G4UIcmdWithADouble("/det/Angle", this);
        fTargetAngleCmd->SetGuidance("Set the target angle.");
        fTargetAngleCmd->SetParameterName("angleD", true);
        // fTargetAngleCmd->SetDefaultUnit("deg");
    }

    DetectorConstructionMessenger::~DetectorConstructionMessenger()
    {
        delete fTargetThicknessCmd;
        delete fTargetAngleCmd;
    }

    void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
    {
        if (command == fTargetThicknessCmd)
        {
            G4double thickness = fTargetThicknessCmd->GetNewDoubleValue(newValue);
            G4cout << "Command received: /mydet/targetThickness " << thickness << G4endl;
            fDetector->SetTargetThickness(thickness);
        }
        else if (command == fTargetAngleCmd)
        {
            G4double angleD = fTargetAngleCmd->GetNewDoubleValue(newValue);
            G4cout << "Command received: /mydet/Angle " << angleD << G4endl;
            fDetector->SetTargetAngle(angleD);
        }
    }
}
