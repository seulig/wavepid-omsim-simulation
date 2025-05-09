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
    //OMSimCommandArgsTable &args = OMSimCommandArgsTable::getInstance();
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    //G4ParticleDefinition* particle = particleTable->FindParticle("mu-");

    OMSimCommandArgsTable &args = OMSimCommandArgsTable::getInstance();
    std::string primary_particle = args.get<std::string>("primary_particle");
    G4ParticleDefinition* particle = particleTable->FindParticle(primary_particle);


    fParticleGun->SetParticleDefinition(particle);

    G4double primary_energy = OMSimCommandArgsTable::getInstance().get<G4double>("primary_energy");
    fParticleGun->SetParticleEnergy(primary_energy * GeV);
    //fParticleGun->SetParticleEnergy(5 * GeV);
    G4double dom_event_spacing = OMSimCommandArgsTable::getInstance().get<G4double>("dom_event_spacing");
    //G4ThreeVector position(-1.2*dom_event_spacing*m, 0.0*m, 0.0*m);
    G4ThreeVector position(-1.7*dom_event_spacing*m, 0.0*m, 0.0*m);//edited for electron wrt stopping power
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