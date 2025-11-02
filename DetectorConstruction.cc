#include "DetectorConstruction.hh"

#include "DetectorMessenger.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SolidStore.hh"
#include "G4StateManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Tubs.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
  ComputeCalorParameters();

  fWorldSizeX = 7. * m;
  fWorldSizeYZ = 7. * m;
  fWorldSizeZ = 7. * m;

  // materiales
  DefineMaterials();
  SetAbsorberMaterial("G4_Pb");
  SetGapMaterial("G4_lAr");

  // comandos interactivos
  fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
  delete fDetectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructCalorimeter();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // NIST repository
  G4NistManager* man = G4NistManager::Instance();
  fDefaultMaterial = man->FindOrBuildMaterial("G4_AIR");
  man->FindOrBuildMaterial("G4_Pb");
  man->FindOrBuildMaterial("G4_lAr");
  //my water tank 
  man->FindOrBuildMaterial("G4_WATER");

  // scint material
  man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructCalorimeter()
{
  // rm old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // World dimensions, you can set different values
  fSolidWorld = new G4Box("World", fWorldSizeX/2, fWorldSizeYZ/2, fWorldSizeZ/2);
  fLogicWorld = new G4LogicalVolume(fSolidWorld, fDefaultMaterial, "World");
  fPhysiWorld = new G4PVPlacement(
    nullptr, G4ThreeVector(), fLogicWorld, "World", nullptr, false, 0, true);

  // scintillator material
  auto scint = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // DIMENSIONS OF THE SCINTILLATOR (VERY IMPORTANT)
G4double barXY = 5*cm;
G4double barZ = 0.5*cm;

  // LOGICAL VOLUME OF THE SCINTILLATOR
  auto solidBar = new G4Box("scintBar", barXY/2, barXY/2, barZ/2);
  auto logicBar = new G4LogicalVolume(solidBar, scint, "ScintillatorLV");

  // VIS ATTRIBUTES
  auto visWorld = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  visWorld->SetVisibility(false);
  fLogicWorld->SetVisAttributes(visWorld);

  auto visBar = new G4VisAttributes(G4Colour(0.1,0.7,0.1));
  visBar->SetForceSolid(true);
  logicBar->SetVisAttributes(visBar);
  
//WATER TANK CONFIG (I ENDED UP CHOOSING TUBS INSTEAD OF BLOCKS)
auto water = G4Material::GetMaterial("G4_WATER");  
//G4double tankX = 1.*m;
//G4double tankY = 1.*m;
//G4double tankZ = 2.*m;
const G4double rIn = 0.*m;
const G4double rOut = 1.*m;
const G4double halfZ = 1.3*m;


//LOGICAL VOLUME OF THE TANK
auto solidTank = new G4Tubs("WaterTank", rIn, rOut, halfZ, 0.*deg, 360.*deg);
auto logicTank = new G4LogicalVolume(solidTank, water, "WaterTankLV");
//VIS
auto visTank = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.4));
visTank->SetForceSolid(true);
logicTank->SetVisAttributes(visTank);

G4double posX = 200*cm;
G4double posY = 0.7*cm;
G4double posZ = 200*cm;

new G4PVPlacement(
    nullptr,
    G4ThreeVector(posX, posY, posZ),
    logicTank,
    "WaterTank",
    fLogicWorld,
    false,
    0,
    true

);

// DETECTOR CONFIGURATION (MADE UP Of SCINTILLATORS )
G4int nDetectors = 2;              
G4int nLayersPerDetector = 1;      
G4int nRowsY = 3;                  
G4int nColumnsX = 8;               

G4double detectorSpacing = 15*cm;  
G4double smallGap = 2*mm;
G4double rowSpacingY = barXY + smallGap;      
G4double columnSpacingX = barXY+ smallGap;   


for (G4int iDet = 0; iDet < nDetectors; iDet++) {
    G4double posZ = iDet * detectorSpacing;

    for (G4int iRow = 0; iRow < nRowsY; iRow++) {
        G4double posY = (iRow - (nRowsY-1)/2.0) * rowSpacingY;

        for (G4int iCol = 0; iCol < nColumnsX; iCol++) {
            G4double posX = (iCol - (nColumnsX-1)/2.0) * columnSpacingX;

            new G4PVPlacement(
                nullptr,
                G4ThreeVector(posX, posY, posZ),
                logicBar,
                "scintBar",
                fLogicWorld,
                false,
                iDet*1000 + iRow*100 + iCol,
                true
                    );
               }
           }
       }
return fPhysiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void DetectorConstruction::SetAbsorberMaterial(const G4String&) {}
void DetectorConstruction::SetAbsorberThickness(G4double) {}
void DetectorConstruction::SetGapMaterial(const G4String&) {}
void DetectorConstruction::SetGapThickness(G4double) {}
void DetectorConstruction::SetCalorSizeYZ(G4double) {}
void DetectorConstruction::SetNbOfLayers(G4int) {}
void DetectorConstruction::PrintCalorParameters() {}