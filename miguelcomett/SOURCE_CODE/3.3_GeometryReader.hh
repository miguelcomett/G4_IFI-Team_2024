#ifndef STLGEOMETRYREADER_HH
#define STLGEOMETRYREADER_HH

#include "G4VSolid.hh"
#include <string>

class STLGeometryReader 
{
    public:
        // Constructor
        STLGeometryReader() = default;
        // Destructor
        ~STLGeometryReader() = default;

        // M�todo para crear un s�lido G4 a partir de un archivo STL
        G4VSolid * CreateSolidFromSTL(const std::string & filename);
};
#endif