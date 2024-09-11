#ifndef G4_PCM_DETECTOR_CONSTRUCTION_MESSENGER_H
#define G4_PCM_DETECTOR_CONSTRUCTION_MESSENGER_H 1

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

namespace G4_PCM
{
    class DetectorConstruction;

    class DetectorConstructionMessenger : public G4UImessenger
    {
    public:
        DetectorConstructionMessenger(DetectorConstruction* detector);
        ~DetectorConstructionMessenger() override;

        void SetNewValue(G4UIcommand* command, G4String newValue) override;

    private:
        DetectorConstruction* fDetector;
        G4UIcmdWithADoubleAndUnit* fTargetThicknessCmd;
    };
}

#endif
