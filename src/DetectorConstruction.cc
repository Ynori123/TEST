#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    G4NistManager* nist = G4NistManager::Instance();

    // 定义材料：世界用空气，自建 BGO（Bi4Ge3O12），密度 7.12 g/cm3
    auto matAir = nist->FindOrBuildMaterial("G4_AIR");
    auto elBi = nist->FindOrBuildElement("Bi");
    auto elGe = nist->FindOrBuildElement("Ge");
    auto elO  = nist->FindOrBuildElement("O");
    auto matBGO = new G4Material("BGO", 7.12*g/cm3, 3);
    matBGO->AddElement(elBi, 4);
    matBGO->AddElement(elGe, 3);
    matBGO->AddElement(elO, 12);

    // 缩小后的世界体（非等边）：x/y 半长 10 cm，z 半长 80 cm
    G4double worldHalfX = 20*cm;
    G4double worldHalfY = 20*cm;
    G4double worldHalfZ = 80*cm;
    auto solidWorld = new G4Box("World", worldHalfX, worldHalfY, worldHalfZ);
    auto logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
    auto physWorld  = new G4PVPlacement(nullptr, {}, logicWorld, "World", nullptr, false, 0);

    // 一根 BGO 晶体：2.5 cm × 2.5 cm × 60 cm（沿 z）
    G4double sizeX = 2.5*cm;
    G4double sizeY = 2.5*cm;
    G4double sizeZ = 60.0*cm;
    auto solidBGO = new G4Box("BGO", 0.5*sizeX, 0.5*sizeY, 0.5*sizeZ);
    auto logicBGO = new G4LogicalVolume(solidBGO, matBGO, "BGO");
    auto visBGO   = new G4VisAttributes(G4Colour(1.0, 0.8, 0.2)); // alpha=0.3 ≈ 70% 透明
    visBGO->SetForceSolid(true);
    logicBGO->SetVisAttributes(visBGO);

    // 放置在世界中心
    new G4PVPlacement(nullptr, G4ThreeVector(), logicBGO, "BGO", logicWorld, false, 0);

    return physWorld;
}
