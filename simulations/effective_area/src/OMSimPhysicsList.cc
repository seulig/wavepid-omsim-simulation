// OMSimPhysicsList.cc
#include "OMSimPhysicsList.hh"
#include "OMSimOpBoundaryProcess.hh"

#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4MuonMinus.hh>
#include <G4MuonPlus.hh>
#include <G4PionMinus.hh>
#include <G4PionPlus.hh>
#include <G4PionZero.hh>
#include <G4Positron.hh>
#include <G4Proton.hh>
#include <G4OpticalPhoton.hh>
#include <G4NeutrinoE.hh>
#include <G4NeutrinoMu.hh>
#include <G4AntiNeutrinoE.hh>
#include <G4AntiNeutrinoMu.hh>
#include <G4Decay.hh>
#include <G4DecayPhysics.hh>
#include <G4EmStandardPhysics.hh>

#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4ePairProduction.hh>
#include <G4Cerenkov.hh>
#include <G4Scintillation.hh>
#include <G4PhotoElectricEffect.hh>
#include <G4ComptonScattering.hh>
#include <G4GammaConversion.hh>
#include <G4GammaConversionToMuons.hh>

#include <G4OpRayleigh.hh>
#include <G4OpMieHG.hh>
#include <G4OpAbsorption.hh>
#include <G4ProcessManager.hh>
#include <G4PhysicsListHelper.hh>
#include <G4SystemOfUnits.hh>

// Include Muon Physics Headers
#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>
#include <G4MuMultipleScattering.hh>
#include <G4eMultipleScattering.hh>

// Include Hadronic Physics Headers for Pions
#include <G4HadronElastic.hh>
#include <G4HadronInelasticProcess.hh>
#include <G4HadronInelasticQBBC.hh>
#include <G4hIonisation.hh>
#include <G4hMultipleScattering.hh>

OMSimPhysicsList::OMSimPhysicsList() : G4VUserPhysicsList() {
    defaultCutValue = 0.1 * mm;
    SetVerboseLevel(1);
}

void OMSimPhysicsList::ConstructParticle() {
    // Define optical photon
    G4OpticalPhoton::OpticalPhotonDefinition();

    // Define EM particles
    G4Gamma::GammaDefinition();
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4Proton::ProtonDefinition();
    G4MuonMinus::MuonMinusDefinition();
    G4MuonPlus::MuonPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4PionPlus::PionPlusDefinition();
    G4PionZero::PionZeroDefinition();

    // Define neutrinos
    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
}

void OMSimPhysicsList::ConstructProcess() {
    AddTransportation();

    // Configure electromagnetic parameters
    // G4EmParameters::Instance()->SetBremsstrahlungThresholdEnergy(1 * MeV);
    // G4EmParameters::Instance()->SetStepFunction(0.5, 5 * mm);

    // Add EM and optical processes
    G4PhysicsListHelper* physicsHelper = G4PhysicsListHelper::GetPhysicsListHelper();

    auto theParticleIterator = GetParticleIterator();
    theParticleIterator->reset();
    while ((*theParticleIterator)()) {
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        G4String particleName = particle->GetParticleName();

        if (!pmanager) {
            G4cout << "Error: No ProcessManager for particle " << particleName << G4endl;
            continue;
        }

        if (particleName == "e-") {
            // Add EM processes for electrons
            physicsHelper->RegisterProcess(new G4eIonisation(), particle);
            physicsHelper->RegisterProcess(new G4eMultipleScattering(), particle);
            physicsHelper->RegisterProcess(new G4eBremsstrahlung(), particle);
            physicsHelper->RegisterProcess(new G4ePairProduction(), particle);
            
            G4cout << "Physics set for e-"  << G4endl;
            
             // Add Cerenkov process for electrons
            G4Cerenkov* cerenkovProcess = new G4Cerenkov();
            cerenkovProcess->SetMaxNumPhotonsPerStep(50);
            cerenkovProcess->SetTrackSecondariesFirst(true);
            pmanager->AddProcess(cerenkovProcess);
            pmanager->SetProcessOrdering(cerenkovProcess, idxPostStep);
            
        } 
        if (particleName == "e+") {
            // Add EM processes for electrons
            physicsHelper->RegisterProcess(new G4eIonisation(), particle);
            physicsHelper->RegisterProcess(new G4eMultipleScattering(), particle);
            physicsHelper->RegisterProcess(new G4eBremsstrahlung(), particle);
            physicsHelper->RegisterProcess(new G4ePairProduction(), particle);
            
            G4cout << "Physics set for e+"  << G4endl;

             // Add Cerenkov process for positrons
            G4Cerenkov* cerenkovProcess = new G4Cerenkov();
            cerenkovProcess->SetMaxNumPhotonsPerStep(50);
            cerenkovProcess->SetTrackSecondariesFirst(true);
            pmanager->AddProcess(cerenkovProcess);
            pmanager->SetProcessOrdering(cerenkovProcess, idxPostStep);
            
        } 
        else if (particleName == "mu-") {
            // Add EM processes for muons
            G4MuMultipleScattering* muMsc = new G4MuMultipleScattering();
            G4MuIonisation* muIoni = new G4MuIonisation();
            G4MuBremsstrahlung* muBrem = new G4MuBremsstrahlung();
            G4MuPairProduction* muPairProd = new G4MuPairProduction();

            physicsHelper->RegisterProcess(muMsc, particle);
            physicsHelper->RegisterProcess(muIoni, particle);
            physicsHelper->RegisterProcess(muBrem, particle);
            physicsHelper->RegisterProcess(muPairProd, particle);

            G4cout << "Physics set for mu-"  << G4endl;

            // Add Cerenkov process for muons
            G4Cerenkov* cerenkovProcess = new G4Cerenkov();
            cerenkovProcess->SetMaxNumPhotonsPerStep(50);
            cerenkovProcess->SetTrackSecondariesFirst(true);
            //cerenkovProcess->SetVerboseLevel(2);  // Keep this for debugging!

            pmanager->AddProcess(cerenkovProcess);
            pmanager->SetProcessOrdering(cerenkovProcess, idxPostStep);
        }
         
        else if (particleName == "mu+") {
            // Add EM processes for muons
            G4MuMultipleScattering* muMsc = new G4MuMultipleScattering();
            G4MuIonisation* muIoni = new G4MuIonisation();
            G4MuBremsstrahlung* muBrem = new G4MuBremsstrahlung();
            G4MuPairProduction* muPairProd = new G4MuPairProduction();

            physicsHelper->RegisterProcess(muMsc, particle);
            physicsHelper->RegisterProcess(muIoni, particle);
            physicsHelper->RegisterProcess(muBrem, particle);
            physicsHelper->RegisterProcess(muPairProd, particle);

            G4cout << "Physics set for mu+"  << G4endl;

            // Add Cerenkov process for anti-muons
            G4Cerenkov* cerenkovProcess = new G4Cerenkov();
            cerenkovProcess->SetMaxNumPhotonsPerStep(50);
            cerenkovProcess->SetTrackSecondariesFirst(true);
            //cerenkovProcess->SetVerboseLevel(2);  // Keep this for debugging!

            pmanager->AddProcess(cerenkovProcess);
            pmanager->SetProcessOrdering(cerenkovProcess, idxPostStep);
        }
        
        else if (particleName == "gamma") {
            // Add processes for gamma photons
            physicsHelper->RegisterProcess(new G4PhotoElectricEffect(), particle);
            physicsHelper->RegisterProcess(new G4ComptonScattering(), particle);
            physicsHelper->RegisterProcess(new G4GammaConversion(), particle);
            physicsHelper->RegisterProcess(new G4GammaConversionToMuons(), particle);
            
            G4cout << "Physics set for gamma"  << G4endl;
        } 
        else if (particleName == "opticalphoton") {
            // Add optical processes
            pmanager->AddDiscreteProcess(new G4OpAbsorption());
            pmanager->AddDiscreteProcess(new G4OpBoundaryProcess());
            pmanager->AddDiscreteProcess(new G4OpRayleigh());
            pmanager->AddDiscreteProcess(new G4OpMieHG());
        }

        // Add Decay Process for applicable particles
        if (particle->GetPDGCharge() != 0.0 && particle->GetPDGLifeTime() > 0) {
            pmanager->AddProcess(new G4Decay(), idxPostStep);
            pmanager->SetProcessOrdering(new G4Decay(), idxPostStep);
        }
    }

    //Remove the loop at the end adding Cerenkov, since it is already done in the loop above
    // Add Cherenkov and Scintillation for all applicable particles
    //G4Cerenkov* cerenkovProcess = new G4Cerenkov();
    //cerenkovProcess->SetMaxNumPhotonsPerStep(300);
    //cerenkovProcess->SetTrackSecondariesFirst(true);

    //G4Scintillation* scintillationProcess = new G4Scintillation();
    //scintillationProcess->SetTrackSecondariesFirst(true);

    theParticleIterator->reset();
    while ((*theParticleIterator)()) {
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();

        if (!pmanager) {
            G4cout << "Error: No ProcessManager for particle " << particle->GetParticleName() << G4endl;
            continue;
        }

    //    if (cerenkovProcess->IsApplicable(*particle)) {
    //        pmanager->AddProcess(cerenkovProcess);
    //        pmanager->SetProcessOrdering(cerenkovProcess, idxPostStep);
    //    }

    //    if (scintillationProcess->IsApplicable(*particle)) {
    //        pmanager->AddProcess(scintillationProcess);
    //        pmanager->SetProcessOrderingToLast(scintillationProcess, idxAtRest);
    //        pmanager->SetProcessOrderingToLast(scintillationProcess, idxPostStep);
    //    }
    }

    G4cout << "Physics processes successfully constructed." << G4endl;
}

void OMSimPhysicsList::SetCuts() {
    SetCutsWithDefault();
    if (verboseLevel > 0) DumpCutValuesTable();
}