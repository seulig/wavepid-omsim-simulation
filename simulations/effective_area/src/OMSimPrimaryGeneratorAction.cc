// File: /simulations/effective_area/src/OMSimPrimaryGeneratorAction.cc
#include "OMSimPrimaryGeneratorAction.hh"

#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4SystemOfUnits.hh>
#include <G4Event.hh>
#include "OMSimCommandArgsTable.hh"
#include "OMSimPMTConstruction.hh"
#include "OMSimOpticalModule.hh"
#include "OMSimPDOM.hh"


/**
 * @brief Constructs the world volume (sphere).
 */



#include <Randomize.hh>

OMSimPrimaryGeneratorAction::OMSimPrimaryGeneratorAction() {
    fParticleGun = new G4ParticleGun(1);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("e-");

    if (!particle) {
        G4Exception("PrimaryGeneratorAction", "InvalidParticle", FatalException, "Particle type e- not found");
    }

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(5 * GeV);

    G4ThreeVector position(-1.1*m, 0.0*m, 0.2*m);
    fParticleGun->SetParticlePosition(position);

    G4ThreeVector direction(1, 0.0, 0.0);
    fParticleGun->SetParticleMomentumDirection(direction.unit());

}

OMSimPrimaryGeneratorAction::~OMSimPrimaryGeneratorAction() {
    delete fParticleGun;
}

void OMSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    fParticleGun->GeneratePrimaryVertex(anEvent);
}