#include "OMSimHitManager.hh"
#include "OMSimCommandArgsTable.hh"
#include "OMSimEventAction.hh"
#include "OMSimLogger.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include <numeric>
#include <stdexcept>

G4Mutex OMSimHitManager::m_mutex = G4Mutex();
OMSimHitManager *OMSimHitManager::m_instance = nullptr;
G4ThreadLocal OMSimHitManager::ThreadLocalData *OMSimHitManager::m_threadData = nullptr;


OMSimHitManager::OMSimHitManager(): m_currentIndex(-1), fROOTManager(nullptr)
{
};

/**
 * @brief Initializes the global instance of OMSimHitManager.
 * 
 * This method is normally called in OMSim::initialiseSimulation.
 */
void OMSimHitManager::init(const std::string& filename)
{
	if (!g_hitManager) g_hitManager = new OMSimHitManager();
    g_hitManager->fROOTManager = new ROOTHitManager(filename);
}

/**
 * @brief Deletes the global instance of OMSimHitManager.
 * 
 * This method is normally called in the destructor ~OMSim.
 */
void OMSimHitManager::shutdown()
{
	if (g_hitManager) {
        delete g_hitManager->fROOTManager;
        delete g_hitManager;
        g_hitManager = nullptr;
    }
}


/**
 * @return A reference to the global OMSimHitManager instance.
 * @throws assert if instance exists (i.e. if it was called before init() or after shutdown()).
 */
OMSimHitManager &OMSimHitManager::getInstance()
{
    if (!g_hitManager)
        throw std::runtime_error("OMSimHitManager accessed before initialization or after shutdown!");
	return *g_hitManager;
}

/**
 * @brief Applies a given permutation to a container.
 *
 * This function rearranges the elements of a vector according to a given permutation.
 * It achieves this using cycles to minimize the number of moves. The function assumes that
 * `p_permutation` holds a permutation of indices into `p_vector`.
 * @param p_vector The vector to which the permutation will be applied.
 * @param p_permutation A permutation represented as a vector of indices.
 */
template <typename T>
void applyPermutation(std::vector<T> &p_vector, const std::vector<std::size_t> &p_permutation)
{
	std::vector<bool> done(p_vector.size()); // Vector to keep track of which positions have been fixed.
	for (std::size_t i = 0; i < p_vector.size(); ++i)
	{
		if (done[i])
			continue;	 // Skip the item if it's already in place.
		done[i] = true; // Mark the item as placed.
		std::size_t prev_j = i;
		std::size_t j = p_permutation[i];
		while (i != j)
		{ // Continue moving items in the cycle until the entire cycle is complete.
			std::swap(p_vector[prev_j], p_vector[j]);
			done[j] = true;
			prev_j = j;
			j = p_permutation[j];
		}
	}
}

/**
 * @brief Appends hit information for a detected photon to the corresponding module's hit data.
 *
 * This method appends hit information to the corresponding module's `HitStats` structure in the manager.
 * If the specified module number is not yet in the manager, a new `HitStats` structure is created for it.
 *
 * @param p_globalTime Time of detection.
 * @param p_localTime Photon flight time.
 * @param p_trackLength Length of the photon's path before hitting.
 * @param p_energy Energy of the detected photon.
 * @param p_PMTHitNumber ID of the PMT that detected the photon.
 * @param p_momentumDirection Momentum direction of the photon at the time of detection.
 * @param p_globalPos Global position of the detected photon.
 * @param p_localPos Local position of the detected photon within the PMT.
 * @param p_distance Distance between generation and detection of photon.
 * @param p_response PMT's p_response to the detected photon, encapsulated as a `PMTPulse`.
 * @param p_moduleNumber ID of the module in which the photon was detected.
 */

void OMSimHitManager::appendHitInfo(
    G4int p_eventid,
    G4double p_globalTime,
    G4double p_entryTime,  // NEW parameter: time when the photon entered the DOM volume
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
    G4int pModuleIndex)
{
    if (!m_threadData)
    {
        log_debug("Initialized m_threadData for thread {} seed {}", G4Threading::G4GetThreadId(),  G4Random::getTheSeed());
        m_threadData = new ThreadLocalData();
    }

    // Check if the module exists in the thread-local map; if not, create it.
    if (m_threadData->moduleHits.find(pModuleIndex) == m_threadData->moduleHits.end())
    {
        m_threadData->moduleHits[pModuleIndex] = HitStats();
    }

    auto &moduleHits = m_threadData->moduleHits[pModuleIndex];

    // Append all information including the new entry time.
    moduleHits.eventId.push_back(p_eventid);
    moduleHits.hitTime.push_back(p_globalTime);
    
    moduleHits.flightTime.push_back(p_localTime);
    moduleHits.pathLenght.push_back(p_trackLength);
    moduleHits.energy.push_back(p_energy);
    moduleHits.PMTnr.push_back(pPMTHitNumber);
    moduleHits.direction.push_back(pMomentumDirection);
    moduleHits.globalPosition.push_back(pGlobalPos);
    moduleHits.localPosition.push_back(pLocalPos);
    moduleHits.generationDetectionDistance.push_back(p_distance);
    moduleHits.PMTresponse.push_back(pResponse);
    moduleHits.photonOrigin.push_back(pPhotonOrigin);
    moduleHits.parentID.push_back(pParentID);
    moduleHits.parentType.push_back(pParentType);
    moduleHits.parentProcess.push_back(pParentProcess);
    // NEW: Save the entry time.
    moduleHits.entryTime.push_back(p_entryTime);

    log_trace("Saved hit nr {} on module {} sensor {} (thread {})", 
              moduleHits.eventId.size(), pModuleIndex, pPMTHitNumber, G4Threading::G4GetThreadId());

    // Create a PhotonInfoROOT object for ROOT output.
    PhotonInfoROOT info;
    info.eventID = p_eventid;
    info.hitTime = p_globalTime;
    info.flightTime = p_localTime;
    info.pathLenght = p_trackLength;
    info.generationDetectionDistance = p_distance;
    info.energy = p_energy;
    info.PMTnr = pPMTHitNumber;
    info.dir_x = pMomentumDirection.x();
    info.dir_y = pMomentumDirection.y();
    info.dir_z = pMomentumDirection.z();
    info.localPos_x = pLocalPos.x();
    info.localPos_y = pLocalPos.y();
    info.localPos_z = pLocalPos.z();
    info.globalPos_x = pGlobalPos.x();
    info.globalPos_y = pGlobalPos.y();
    info.globalPos_z = pGlobalPos.z();
    info.deltaPos_x = p_distance; // Or calculate appropriately
    info.deltaPos_y = 0;         // Set appropriately if needed
    info.deltaPos_z = 0;         // Set appropriately if needed
    info.wavelength = p_wavelength;
    info.parentID = pParentID;
    info.parentType = pParentType;
    info.photonOrigin = pPhotonOrigin;
    info.parentProcess = pParentProcess;
    // NEW: Record the entry time in the ROOT structure.
    info.entryTime = p_entryTime;

    // Fill the ROOT tree with the hit information.
    if (fROOTManager) {
        fROOTManager->FillPhotonInfo(info);
    } else {
        log_error("ROOTHitManager not initialized!");
    }
}

/**
 * @brief Stores the number of PMTs in a module for correct data handling
 * @param p_numberOfPMTs Nr of PMTs in OM
 * @param pModuleIndex Module index for which we are getting the information (default 0)
 */
void OMSimHitManager::setNumberOfPMTs(int pNumberOfPMTs, int pModuleIndex)
{
	log_trace("Setting number of PMTs to {} in module with index {}", pNumberOfPMTs, pModuleIndex);
	m_numberOfPMTs[pModuleIndex] = pNumberOfPMTs;
}

/**
 * @brief Retrieves the HitStats structure for the specified module, should be called after data between threads was merged. @see mergeThreadData
 * @param pModuleIndex Index of the module for which to retrieve hit statistics. Default is 0.
 * @return A HitStats structure containing hit information of specified module.
 */
HitStats OMSimHitManager::getMergedHitsOfModule(int pModuleIndex)
{
	return m_moduleHits[pModuleIndex];
}




/**
 * @brief Retrieves the HitStats structure for the specified module of single thread.
 * @param pModuleIndex Index of the module for which to retrieve hit statistics. Default is 0.
 * @return A HitStats structure containing hit information of specified module.
 */
HitStats OMSimHitManager::getSingleThreadHitsOfModule(int pModuleIndex)
{
	log_debug("Getting m_threadData of module {} (thread {})", pModuleIndex, G4Threading::G4GetThreadId());
	return m_threadData->moduleHits[pModuleIndex];
}


bool OMSimHitManager::areThereHitsInModuleSingleThread(int pModuleIndex)
{
	if (!m_threadData) {return false;};
	return m_threadData->moduleHits.find(pModuleIndex) != m_threadData->moduleHits.end();
}

/**
 * @brief Deletes hit information in memory for all modules.
 */
void OMSimHitManager::reset()
{
	log_trace("Reseting hit manager");
	//m_moduleHits.clear();
	if (m_threadData)
	{
		log_trace("Deleting m_threadData of Thread ID {}", G4Threading::G4GetThreadId());
		m_threadData->moduleHits.clear();
		delete m_threadData;
		m_threadData = nullptr;
	}
	m_moduleHits.clear();
	log_trace("Finished reseting hit manager");
}

/**
 * @brief Counts hits for a specified module.
 * @param pModuleIndex Index of the module for which to count hits. Default is 0.
 * @param p_getWeightedDE If true, the counts are weighted with detection probability
 * @return A vector containing the hit count for each PMT in the specified module.
 */
std::vector<double> OMSimHitManager::countMergedHits(int pModuleIndex, bool p_getWeightedDE)
{
	log_trace("Counting number of detected photons in module with index {}", pModuleIndex);
	HitStats hitsOfModule = m_moduleHits[pModuleIndex];
	G4int numberOfPMTs = m_numberOfPMTs[pModuleIndex];

	std::vector<double> hits(numberOfPMTs + 1, 0.0);
	for (int i = 0; i < (int)hitsOfModule.PMTnr.size(); i++)
	{
		double newcount = (p_getWeightedDE) ?  hitsOfModule.PMTresponse.at(i).detectionProbability : 1;
		hits[hitsOfModule.PMTnr.at(i)] += newcount;
		hits[numberOfPMTs] += newcount;
	}

	return hits;
}

/**
 * @brief Sorts the hit statistics by the hit time.
 * @param p_hits The hit statistics to be sorted.
 */
void OMSimHitManager::sortHitStatsByTime(HitStats &p_hits)
{
	// Create a vector of indices
	std::vector<std::size_t> indices(p_hits.hitTime.size());
	std::iota(indices.begin(), indices.end(), 0); // Fill it with 0, 1, ... N-1

	// Sort the indices vector according to hitTime
	std::sort(indices.begin(), indices.end(),
			  [&p_hits](std::size_t a, std::size_t b)
			  {
				  return p_hits.hitTime[a] < p_hits.hitTime[b];
			  });

	// Now, apply the permutation function to every vector in the struct
	applyPermutation(p_hits.eventId, indices);
	applyPermutation(p_hits.hitTime, indices);
	applyPermutation(p_hits.flightTime, indices);
	applyPermutation(p_hits.pathLenght, indices);
	applyPermutation(p_hits.energy, indices);
	applyPermutation(p_hits.PMTnr, indices);
	applyPermutation(p_hits.direction, indices);
	applyPermutation(p_hits.localPosition, indices);
	applyPermutation(p_hits.globalPosition, indices);
	applyPermutation(p_hits.generationDetectionDistance, indices);
	applyPermutation(p_hits.PMTresponse, indices);
    applyPermutation(p_hits.photonOrigin, indices);
    applyPermutation(p_hits.parentID, indices);
}

/**
 * @brief Calculates the multiplicity of hits within a specified time window for a given module.
 *
 * This method determines the multiplicity of hits for the specified optical module within a
 * given time window. Multiplicity is defined as the number of PMTs detecting a hit within the
 * time window. The result is a vector where each element represents the number of occurrences
 * of a specific multiplicity.
 *
 * For instance, if the resulting vector is [5, 3, 2], it means:
 * - 5 occurrences of 1 PMT detecting a hit within the time window.
 * - 3 occurrences of 2 PMTs detecting hits within the same window.
 * - 2 occurrences of 3 PMTs detecting hits within the window.
 *
 * @param pTimeWindow The time window within which to calculate the multiplicity (in seconds).
 * @param pModuleNumber The index of the module for which to calculate the multiplicity. Default is 0.
 * @return A vector containing the multiplicity data.
 */
std::vector<int> OMSimHitManager::calculateMultiplicity(const G4double pTimeWindow, int pModuleNumber)
{
	log_trace("Calculating multiplicity in time window {} for module with index", pTimeWindow, pModuleNumber);

	HitStats hitsOfModule = m_moduleHits[pModuleNumber];
	G4int numberOfPMTs = m_numberOfPMTs[pModuleNumber];

	sortHitStatsByTime(hitsOfModule);

	std::vector<int> multiplicity(numberOfPMTs, 0); // Initialize with zeros and size pPMTCount

	std::size_t skipUntil = 0; // Index up to which we should skip in outer loop
	int vectorSize = hitsOfModule.hitTime.size();

	for (std::size_t i = 0; i < vectorSize - 1; ++i)
	{
		if (i < skipUntil)
		{
			continue;
		}

		std::vector<int> hitPMT(numberOfPMTs, 0);

		hitPMT[hitsOfModule.PMTnr.at(i)] = 1;
		int currentSum = 0;

		// Loop through the next hits to see if they're within the time window
		for (std::size_t j = i + 1; j < vectorSize; ++j)
		{
			if ((hitsOfModule.hitTime.at(j) - hitsOfModule.hitTime.at(i)) > pTimeWindow)
			{
				skipUntil = j;
				break;
			}
			else
			{
				hitPMT[hitsOfModule.PMTnr.at(j)] = 1;
			}
		}

		// Calculate the multiplicity
		for (std::size_t k = 0; k < numberOfPMTs; ++k)
		{
			currentSum += hitPMT[k];
		}
		multiplicity[currentSum - 1] += 1;
	}
	return multiplicity;
}

void OMSimHitManager::mergeThreadData()
{
	log_trace("Merge thread data was called");
	G4AutoLock lock(&m_mutex);
	if (m_threadData)
	{
		log_debug("Merging data for thread {}", G4Threading::G4GetThreadId());
		for (const auto &[moduleIndex, hits] : m_threadData->moduleHits)
		{
			auto &globalHits = m_moduleHits[moduleIndex];

			log_debug("Thread ID: {} - Module Index: {} - Hit vector sizes: ={} - Merged size prior {}",
					  G4Threading::G4GetThreadId(), moduleIndex, hits.eventId.size(), globalHits.eventId.size());

			globalHits.eventId.insert(globalHits.eventId.end(), hits.eventId.begin(), hits.eventId.end());
			globalHits.hitTime.insert(globalHits.hitTime.end(), hits.hitTime.begin(), hits.hitTime.end());
			globalHits.entryTime.insert(globalHits.entryTime.end(), hits.entryTime.begin(), hits.entryTime.end());
			globalHits.flightTime.insert(globalHits.flightTime.end(), hits.flightTime.begin(), hits.flightTime.end());
			globalHits.pathLenght.insert(globalHits.pathLenght.end(), hits.pathLenght.begin(), hits.pathLenght.end());
			globalHits.energy.insert(globalHits.energy.end(), hits.energy.begin(), hits.energy.end());
			globalHits.PMTnr.insert(globalHits.PMTnr.end(), hits.PMTnr.begin(), hits.PMTnr.end());
			globalHits.direction.insert(globalHits.direction.end(), hits.direction.begin(), hits.direction.end());
			globalHits.localPosition.insert(globalHits.localPosition.end(), hits.localPosition.begin(), hits.localPosition.end());
			globalHits.globalPosition.insert(globalHits.globalPosition.end(), hits.globalPosition.begin(), hits.globalPosition.end());
			globalHits.generationDetectionDistance.insert(globalHits.generationDetectionDistance.end(), hits.generationDetectionDistance.begin(), hits.generationDetectionDistance.end());
			globalHits.PMTresponse.insert(globalHits.PMTresponse.end(), hits.PMTresponse.begin(), hits.PMTresponse.end());
            globalHits.photonOrigin.insert(globalHits.photonOrigin.end(), hits.photonOrigin.begin(), hits.photonOrigin.end());
            globalHits.parentID.insert(globalHits.parentID.end(), hits.parentID.begin(), hits.parentID.end());
            globalHits.parentProcess.insert(globalHits.parentProcess.end(), hits.parentProcess.begin(), hits.parentProcess.end());
		}
		delete m_threadData;
		m_threadData = nullptr;
	}
}