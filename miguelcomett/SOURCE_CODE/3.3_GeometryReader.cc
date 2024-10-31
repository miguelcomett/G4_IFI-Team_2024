#include <fstream>
#include <sstream>
#include <iostream>

#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4SystemOfUnits.hh"

#include "3.3_GeometryReader.hh"

G4VSolid * STLGeometryReader::CreateSolidFromSTL(const std::string& filename) 
{
    // Crear un s�lido teselado
    G4TessellatedSolid* tessellatedSolid = new G4TessellatedSolid("STLModel");

    // Abrir archivo STL
    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        G4cerr << "No se pudo abrir el archivo STL: " << filename << G4endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        if (line.find("facet normal") != std::string::npos) 
        {
            std::istringstream iss(line);
            std::string discard;
            G4double nx, ny, nz;
            iss >> discard >> discard >> nx >> ny >> nz;  // Leer la normal

            // Leer los tres v�rtices del tri�ngulo
            G4ThreeVector v1, v2, v3;
            for (int i = 0; i < 3; ++i) {
                std::getline(file, line); // Leer la l�nea con el v�rtice
                std::istringstream vss(line);
                std::string vertexDiscard;
                G4double vx, vy, vz;
                vss >> vertexDiscard >> vertexDiscard >> vx >> vy >> vz;  // Obtener los v�rtices
                if (i == 0) v1.set(vx * mm, vy * mm, vz * mm);
                else if (i == 1) v2.set(vx * mm, vy * mm, vz * mm);
                else if (i == 2) v3.set(vx * mm, vy * mm, vz * mm);
            }

            // Crear una faceta triangular y a�adirla al s�lido
            G4TriangularFacet* facet = new G4TriangularFacet(v1, v2, v3, ABSOLUTE);
            tessellatedSolid->AddFacet((G4VFacet*)facet);
        }
    }

    // Cerrar archivo y finalizar el s�lido
    file.close();
    tessellatedSolid->SetSolidClosed(true);

    return tessellatedSolid;
}
