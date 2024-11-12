#include "2.0_PhysicsList.hh"

MyPhysicsList::MyPhysicsList()
{
    RegisterPhysics(new G4EmStandardPhysics(0));
    RegisterPhysics(new G4OpticalPhysics(0));
}

MyPhysicsList::~MyPhysicsList(){}