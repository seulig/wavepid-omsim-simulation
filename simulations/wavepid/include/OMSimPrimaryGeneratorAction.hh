#pragma once

#include <G4VUserPrimaryGeneratorAction.hh>
#include <memory>

class G4ParticleGun;
class G4Event;

class OMSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    OMSimPrimaryGeneratorAction();
    ~OMSimPrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event* anEvent) override;

private:
    G4ParticleGun* fParticleGun; // Declare fParticleGun as a member variable
};