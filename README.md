# My Geant4 Project

## Short description
This Geant4 application simulates the response of a modular scintillator detector system under different particle beams.  
It includes a detailed `DetectorConstruction` class defining the experimental geometry, materials, and sensitive volumes used for energy deposition studies in the **analysis** module.

---

## Requirements
- **Geant4** ≥ 11.3.2 (tested version)  
- **C++17**  
- **CMake** ≥ 3.16  
- **ROOT** (optional, for histogram output)  
- **CLHEP** (included with Geant4 build)

---

## Quick start (build & run)

### 1. Configure Geant4 environment
```bash
source /path/to/geant4/bin/geant4.sh


## BUILD THE PROJECT

mkdir build && cd build
cmake .. -DGeant4_DIR=/path/to/geant4/lib/Geant4-11.3.2
make 

##RUN AN EXAMPLE

./analysis macros/run1.mac

##CONTACT 

matias.toledo.calderon@gmail.com

