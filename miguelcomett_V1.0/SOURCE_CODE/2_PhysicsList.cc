#include "2_PhysicsList.hh"

MyPhysicsList::MyPhysicsList()
{
    RegisterPhysics(new G4EmStandardPhysics());
    RegisterPhysics(new G4OpticalPhysics());
}

MyPhysicsList::~MyPhysicsList(){}