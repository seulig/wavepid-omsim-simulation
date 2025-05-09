// OMSimTrackingAction.hh
#pragma once

#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include <unordered_map>
#include <string>
#include <mutex>

class OMSimTrackingAction : public G4UserTrackingAction {
public:
    OMSimTrackingAction();
    virtual ~OMSimTrackingAction();

    // Overridden methods
    virtual void PreUserTrackingAction(const G4Track* track) override;
    virtual void PostUserTrackingAction(const G4Track* track) override;

    // Methods to retrieve information
    std::string GetParticleType(G4int trackID) const;
    std::string GetCreatorProcess(G4int trackID) const;

    // Singleton access
    static OMSimTrackingAction& GetInstance();

private:
    // Maps to store TrackID to ParticleType and CreatorProcess
    std::unordered_map<G4int, std::string> fTrackIDToParticleTypeMap;
    std::unordered_map<G4int, std::string> fTrackIDToCreatorProcessMap;

    mutable std::mutex fMapMutex; // For thread safety

    // Singleton instance
    static OMSimTrackingAction* instance;
};
