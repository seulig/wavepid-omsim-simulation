// OMSimTrackingAction.cc
#include "OMSimTrackingAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include <stdexcept>

// Initialize the static instance pointer
OMSimTrackingAction* OMSimTrackingAction::instance = nullptr;

// Singleton accessor
OMSimTrackingAction& OMSimTrackingAction::GetInstance() {
    if (!instance) {
        throw std::runtime_error("OMSimTrackingAction instance not created!");
    }
    return *instance;
}

OMSimTrackingAction::OMSimTrackingAction()
    : G4UserTrackingAction(),
      fTrackIDToParticleTypeMap(),
      fTrackIDToCreatorProcessMap()
{
    // Ensure only one instance exists
    if (instance != nullptr) {
        throw std::runtime_error("OMSimTrackingAction is a singleton!");
    }
    instance = this;
}

OMSimTrackingAction::~OMSimTrackingAction()
{
    instance = nullptr;
}

void OMSimTrackingAction::PreUserTrackingAction(const G4Track* track)
{
    std::lock_guard<std::mutex> lock(fMapMutex);
    G4int trackID = track->GetTrackID();
    std::string particleName = track->GetDefinition()->GetParticleName();
    fTrackIDToParticleTypeMap[trackID] = particleName;

    const G4VProcess* creatorProcess = track->GetCreatorProcess();
    if (creatorProcess) {
        std::string processName = creatorProcess->GetProcessName();
        fTrackIDToCreatorProcessMap[trackID] = processName;
    } else {
        fTrackIDToCreatorProcessMap[trackID] = "Primary"; // Mark primaries
    }
}
void OMSimTrackingAction::PostUserTrackingAction(const G4Track* /*track*/)
{
    // Optional: Implement if needed
}

std::string OMSimTrackingAction::GetParticleType(G4int trackID) const
{
    std::lock_guard<std::mutex> lock(fMapMutex);
    auto it = fTrackIDToParticleTypeMap.find(trackID);
    if (it != fTrackIDToParticleTypeMap.end()) {
        return it->second;
    } else {
        return "Unknown";
    }
}

std::string OMSimTrackingAction::GetCreatorProcess(G4int trackID) const
{
    std::lock_guard<std::mutex> lock(fMapMutex);
    auto it = fTrackIDToCreatorProcessMap.find(trackID);
    if (it != fTrackIDToCreatorProcessMap.end()) {
        return it->second;
    } else {
        return "Unknown";
    }
}
