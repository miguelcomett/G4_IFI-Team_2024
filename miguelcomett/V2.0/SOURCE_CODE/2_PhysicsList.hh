#ifndef PHYSICSLISTS_HH
#define PHYSICSLISTS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"

class MyPhysicsList : public G4VModularPhysicsList
{
    public:
        MyPhysicsList();
        ~MyPhysicsList();   
};

#endif