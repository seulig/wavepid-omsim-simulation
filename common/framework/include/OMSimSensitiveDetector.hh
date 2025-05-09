// OMSimSensitiveDetector.hh

#pragma once

#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include "OMSimPMTResponse.hh"

#include <vector>
#include <string>

// Forward declarations to avoid including big headers in the .hh file
class G4Step;
class G4Track;
class G4TouchableHistory;
class G4OpBoundaryProcess; // We'll only store a pointer to this

/**
 * @enum DetectorType
 * @brief Enum of types of detectors supported by OMSimSensitiveDetector.
 */
enum class DetectorType {
    PMT,                  
    VolumePhotonDetector,
    BoundaryPhotonDetector,
    PerfectPMT
};

/**
 * @struct PhotonInfo
 * @brief Contains information about a detected photon which will be appended in HitManager.
 */
struct PhotonInfo {
    G4int eventID;
    G4double globalTime;
    G4double localTime;
    G4double trackLength;
    G4double kineticEnergy;
    G4double wavelength;
    G4ThreeVector globalPosition;
    G4ThreeVector localPosition;
    G4ThreeVector momentumDirection;
    G4ThreeVector deltaPosition;
    G4int pmtNumber;
    G4int detectorID;
    OMSimPMTResponse::PMTPulse PMTResponse;
    G4String photonOrigin;
    G4int parentID;
    G4String parentType;
    G4String parentProcess;
    G4double entryTime;
};

/**
 * @class OMSimSensitiveDetector
 * @brief Represents a sensitive detector.
 */
class OMSimSensitiveDetector : public G4VSensitiveDetector
{
public:
    OMSimSensitiveDetector(G4String pName, DetectorType pDetectorType);
    ~OMSimSensitiveDetector() override;

    G4bool ProcessHits(G4Step* pStep, G4TouchableHistory* pTouchableHistory) override;
    void setPMTResponse(OMSimPMTResponse* pResponse);

private:
    bool              m_QEcut;
    OMSimPMTResponse* m_PMTResponse;
    DetectorType      m_detectorType;

    // We only declare this static thread_local pointer here;
    // the actual definition (with = nullptr) will be in the .cc file
    static thread_local G4OpBoundaryProcess* m_boundaryProcess;

    // Helper methods
    G4bool        checkVolumeAbsorption(G4Step* pStep);
    G4bool        checkBoundaryAbsorption(G4Step* pStep);
    PhotonInfo    getPhotonInfo(G4Step* pStep);
    G4bool        handlePMT(G4Step* pStep, G4TouchableHistory* pTouchableHistory);
    G4bool        handleGeneralPhotonDetector(G4Step* pStep, G4TouchableHistory* pTouchableHistory);
    bool          isPhotonDetected(double p_efficiency);
    void          storePhotonHit(PhotonInfo& pInfo);
    void          fetchBoundaryProcess();
    void          killParticle(G4Track* pTrack);
};
