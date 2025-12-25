#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Event.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    fGun = new G4ParticleGun(1); // 单次发射 1 个粒子/事件
    auto gamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    fGun->SetParticleDefinition(gamma);
    fGun->SetParticleEnergy(1* GeV);
    // fGun->SetParticleMomentumDirection(G4ThreeVector(0., -0.86602, -0.5));
    fGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.0));
    fGun->SetParticlePosition(G4ThreeVector(0., 0., 50.0 *cm));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fGun; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    fGun->GeneratePrimaryVertex(event);
}

