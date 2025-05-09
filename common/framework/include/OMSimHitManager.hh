#pragma once

#include "OMSimPMTResponse.hh"
#include "ROOTHitManager.hh" // Added this include

#include <G4ThreeVector.hh>
#include <fstream>
#include <G4AutoLock.hh>
#include <G4Threading.hh>
#pragma once



/**
 * @struct HitStats
 * @brief A structure of vectors to store information about detected photons.
 * @ingroup common
 */
struct HitStats
{
    std::vector<G4long> eventId;                         ///< ID of the event
    std::vector<G4double> hitTime;                       ///< Time of detection.
    std::vector<G4double> entryTime;                       ///< Time of reachign DOM
    std::vector<G4double> flightTime;                    ///< Photon flight time.
    std::vector<G4double> pathLenght;                    ///< Length of the photon's path before hitting.
    std::vector<G4double> energy;                        ///< Energy of the detected photon.
    std::vector<G4int> PMTnr;                            ///< ID of the PMT that detected the photon.
    std::vector<G4ThreeVector> direction;                ///< Momentum direction of the photon at the time of detection.
    std::vector<G4ThreeVector> localPosition;            ///< Local position of the detected photon within the PMT.
    std::vector<G4ThreeVector> globalPosition;           ///< Global position of the detected photon.
    std::vector<G4double> generationDetectionDistance;   ///< Distance between generation and detection of photon.
    std::vector<OMSimPMTResponse::PMTPulse> PMTresponse; ///< PMT's response to the detected photon, encapsulated as a `PMTPulse`.
    std::vector<G4String> photonOrigin;                  ///< String describing the origin of the photon
    std::vector<G4int> parentID;                  ///< String describing the origin of the photon
    std::vector<G4String> parentType;                    ///< Parent Particle Type (e.g., e-, mu-, etc.)
    std::vector<G4String> parentProcess;                 ///< String describing the process that created the parent particle
};

/**
 * @class OMSimHitManager
 * @brief Manages detected photon information.
 *
 * Stores, manages, and provides access information related to detected photons across multiple optical modules.
 * The manager uses a global instance pattern, ensuring a unified access point for photon hit data. Its lifecycle is managed by the OMSim class.
 *
 * The hits are stored using 'OMSimHitManager::appendHitInfo'.
 * The analysis manager of each study is in charge of writing the stored information into a file (see for example 'OMSimEffectiveAreaAnalyisis::writeScan' or 'OMSimDecaysAnalysis::writeHitInformation').
 * If the simulation will continue after the data is written, do not forget calling 'OMSimHitManager::reset'!.
 *
 * @ingroup common
 */
class OMSimHitManager
{
    OMSimHitManager();
    ~OMSimHitManager() = default;
    OMSimHitManager(const OMSimHitManager &) = delete;
    OMSimHitManager &operator=(const OMSimHitManager &) = delete;

public:
    static void init(const std::string& filename);
    static void shutdown();
    static OMSimHitManager &getInstance();


    void appendHitInfo(
        G4int p_eventid,
        G4double p_globalTime,
        G4double p_entryTime,
        G4double p_localTime,
        G4double p_trackLength,
        G4double p_energy,
        G4int pPMTHitNumber,
        G4ThreeVector pMomentumDirection,
        G4ThreeVector pGlobalPos,
        G4ThreeVector pLocalPos,
        G4double p_distance,
        OMSimPMTResponse::PMTPulse pResponse,
        G4String pPhotonOrigin,
        G4int pParentID,
        std::string pParentType,
        G4String pParentProcess,
        G4double p_wavelength, // Newly added parameter for wavelength
        G4int pModuleIndex);    // Ensure correct naming if different


    void reset();
    std::vector<double> countMergedHits(int pModuleIndex = 0, bool pDEweight = false);
    void setNumberOfPMTs(int pNumberOfPMTs, int pModuleIndex = 0);
    HitStats getMergedHitsOfModule(int pModuleIndex = 0);
    HitStats getSingleThreadHitsOfModule(int pModuleIndex = 0);
    bool areThereHitsInModuleSingleThread(int pModuleIndex = 0);
    void sortHitStatsByTime(HitStats &pHits);
    std::vector<int> calculateMultiplicity(const G4double pTimeWindow, int pModuleNumber = 0);
    G4int getNextDetectorIndex() { return ++m_currentIndex; }
    G4int getNumberOfModules() { return m_currentIndex + 1; }

    void mergeThreadData();

    std::map<G4int, HitStats> m_moduleHits; ///< Map of a HitStats containing hit information for each simulated optical module

private:
    std::map<G4int, G4int> m_numberOfPMTs; ///< Map of number of PMTs in the used optical modules
    G4int m_currentIndex;
    static G4Mutex m_mutex;
    static OMSimHitManager *m_instance;
    struct ThreadLocalData
    {
        std::map<G4int, HitStats> moduleHits;
    };
    G4ThreadLocal static ThreadLocalData *m_threadData;
    ROOTHitManager* fROOTManager = nullptr;
};

inline OMSimHitManager *g_hitManager = nullptr;