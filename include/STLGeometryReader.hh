// STLGeometryReader.hh

#ifndef STLGEOMETRYREADER_HH
#define STLGEOMETRYREADER_HH

#include "G4VSolid.hh"
#include <string>

class STLGeometryReader {
public:
    // Constructor
    STLGeometryReader() = default;
    // Destructor
    ~STLGeometryReader() = default;

    // Método para crear un sólido G4 a partir de un archivo STL
    G4VSolid* CreateSolidFromSTL(const std::string& filename);
};

#endif // STLGEOMETRYREADER_HH
