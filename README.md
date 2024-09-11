# G4-PCM
Author: [Juan Pablo Solís Ruiz] 7/26/2023


# Overview

![Geant4 PCM](PCM.png)

G4 PCM is a simulation program capable of creating material characterizations using photons.

Photon activation analysis (PAA) is a nondestructive materials characterization technique that exploits high-energy photons to retrieve the elemental composition of a large variety of samples. During the irradiation time, photons interact with the sample under study, inducing photonuclear reactions

# How to use

Before using G4 PCM, you must have the Geant4 source code, cmake, and visual studio downloaded. For a comprehensive tutorial on downloading and installing these, check out my [YouTube tutorial](https://youtu.be/w7k9PK1Ipv8). 

To run G4 PCM, download the source code, and compile it with cmake. This will create a build directory with an executible file in the "release" directory. Clicking on this file should open a Geant4 GUI. Once in the GUI, you can run test.mac to display "PCM" in the output console.

To run without the Geant4 GUI, you can go to the directory where G4-Hello-World is and run,
```
.\PCM test.mac
```

# Versions

This app was devloped in Windows 11. It uses Geant4 version 11.1.2, and CMake version 3.16 and above. (Release for macOS coming soon...)

# Features of G4-HPCM:

- [Simple UI or Batch mode Geant4 application](#simple-ui-or-batch-mode-geant4-application)
- [Pre-Configured CMakeLists file](#pre-configured-cmakelists-file)
- [Connection to Geant4 source code](#connection-to-geant4-source-code)
- [Link to src and include folders](#link-to-src-and-include-folders)
- [Automatic mac file copying](#automatic-mac-file-copying)
- [Geant4 License](#geant4-license)
- [Simple gitignore file](#simple-gitignore-file)

## Simple UI or Batch mode Geant4 application

This app supports both UI and Batch modes for Geant4 devloping. In other words, you can either double click on the Hello_World.exe file to run, or run from the terminal by the command, 
```
.\PCM test.mac
```
The second method runs much faster especially when modelling several particle interactions.

## Pre-Configured CMakeLists file

Save hours configuring your CMakeLists.txt file by using G4 PCM. G4 PCM uses similar cmake configurations that can be found in official Geant4 example applications, as well as some helpful additions. 

## Connection to Geant4 source code

As long as you have correctly installed the Geant4 source code on your computer, G4 PCM will automatically link to the Geant4 source code right away. This way you can start including G4 headers without missing a beat. 

## Link to src and include folders

G4 PCM's CMakeLists.txt file automatically configures your project's include and src directories. The project will expect your "src" directory to contain any .cc files where you define your own classes, and it will expect to find your own .hh files in the "include" directory. 

## Automatic mac file copying

An added bonus to G4 PCM is automatic mac file copying. Upon building, G4 PCM will copy all mac files in the "mac_files" directory into the same directory as the .exe file. This way your application can run the mac files without errors. 

## Geant4 License

G4 PCM comes with the Geant4 software License already included, so no need to worry about legal issues when developing open source Geant4 applications. 

## Simple gitignore file

G4 PCM comes with a very simple .gitignore file, which ignores your .vs/, out/ and build/ directories. This allows for simpler version control by making sure all the junk files don't get added to your own GitHub repository. 

# Extra

https://www.nature.com/articles/s42005-021-00685-2
https://gitlab.cern.ch/geant4/geant4/blob/4ec577e5c49e900713a6c89b59a8b3e8a4b71a52/examples/advanced/composite_calorimeter/dataglobal/material.cms#L766
https://www.msesupplies.com/products/pbwo4-crystals-lead-tungstate

# Lista de materiales

https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html


# Wolframium characterization

0.00613050000 - 0.01 keV

        G4double targetThickness = 60 * nm;

0.01 - 0.05 keV

        G4double targetThickness = 80 * nm;

0.05 - 0.1 keV

        G4double targetThickness = 120 * nm;

0.1 - 0.9 keV

        G4double targetThickness = 280 * nm;

0.9 - 3.5 keV

        G4double targetThickness = 700 * nm;

3.5 - 6 keV

        G4double targetThickness = 2200 * nm;

6 - 20 keV

        G4double targetThickness = 9000 * nm;

20 - 25 keV

        G4double targetThickness = 40000 * nm;


25 - 100 keV

        G4double targetThickness = 0.1 * mm;

100 - 200 keV

        G4double targetThickness = 1 * mm;

200 - 300 keV

        G4double targetThickness = 5 * mm;

300 - 432.9451 keV

        G4double targetThickness = 13 * mm;



01000011 01110101 01101001 01100100 01100001 01100100 01101111 00100000 01100101 01110011 01110101 01100011 00100000 01101110 00100000 01101100 00100000 01110011 01110100 01101100 01110011 00100000 01110100 01110001 01110100 01101101 01100011 00100000 01100101 01100101 00100000 01110101 00100000 01100001 00100000 01110000 01110001 01111001 01101100 00100000 01101000 01110010 00100000 01110000 01110010 00100000 01100101 00100000 01100101 00100000 01101100 00100000 01100111 00100000 01110100 01110101 00100000 01110000 01110101 01100101 01100100 01100101 01110011 