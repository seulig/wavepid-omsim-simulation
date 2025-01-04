// File: /simulations/effective_area/src/OMSimEffectiveAreaAnalyisis.cc
#include "OMSimEffectiveAreaAnalyisis.hh"
#include "OMSimCommandArgsTable.hh"
#include "OMSimHitManager.hh"
#include "OMSimTools.hh"
#include <iomanip>

/**
 * @brief Calculates the effective area based on the number of hits and beam properties.
 * @param p_weightTotal The number of hits weighted.
 * @param p_totalCount The number of hits
 * @return Returns a structure with the effective area and its uncertainty.
 */
effectiveAreaResult OMSimEffectiveAreaAnalyisis::calculateEffectiveArea(double p_weightTotal, double p_totalCount)
{
	OMSimCommandArgsTable &args = OMSimCommandArgsTable::getInstance();
	G4double beamRadius = args.get<G4double>("radius")/10.; //in cm
	G4double numberPhotons = args.get<int>("numevents");
	G4double beamArea = CLHEP::pi * beamRadius * beamRadius;
	G4double effectiveArea = p_weightTotal * beamArea / numberPhotons;
	G4double effectiveAreaError = effectiveArea / sqrt(p_totalCount);
	return { effectiveArea, effectiveAreaError };
}

void OMSimEffectiveAreaAnalyisis::writeHitInformation(double pWavelength)
{
    OMSimHitManager &hitManager = OMSimHitManager::getInstance();
    HitStats hits = hitManager.getMergedHitsOfModule();
    G4String fileName = m_outputFileName + "_hits.dat";
    std::fstream dataFile;
    dataFile.open(fileName.c_str(), std::ios::out | std::ios::app);
    dataFile << "# eventID \t hitTime \t flightTime \t pathLenght \t energy \t PMTnr \t directionX \t directionY \t directionZ \t localPositionX \t localPositionY \t localPositionZ \t globalPositionX \t globalPositionY \t globalPositionZ \t generationDetectionDistance \t PE \t transitTime \t detectionProbability \t wavelength" << G4endl;
    for (int i = 0; i < (int)hits.eventId.size(); i++)
    {
        dataFile << hits.eventId.at(i) << "\t";
        dataFile << std::setprecision(13);
        dataFile << hits.hitTime.at(i) / ns << "\t";
        dataFile << hits.flightTime.at(i) / ns << "\t";
        dataFile << hits.pathLenght.at(i) / mm << "\t";
        dataFile << hits.energy.at(i) / eV << "\t";
        dataFile << hits.PMTnr.at(i) << "\t";
        dataFile << hits.direction.at(i).x() << "\t";
        dataFile << hits.direction.at(i).y() << "\t";
        dataFile << hits.direction.at(i).z() << "\t";
        dataFile << hits.localPosition.at(i).x() / mm << "\t";
        dataFile << hits.localPosition.at(i).y() / mm << "\t";
        dataFile << hits.localPosition.at(i).z() / mm << "\t";
        dataFile << hits.globalPosition.at(i).x() / mm << "\t";
        dataFile << hits.globalPosition.at(i).y() / mm << "\t";
        dataFile << hits.globalPosition.at(i).z() / mm << "\t";
        dataFile << hits.generationDetectionDistance.at(i) / mm << "\t";
        
        // Apply post-processing to PE and transitTime
        double pe = hits.PMTresponse.at(i).PE;
        double transitTime = hits.PMTresponse.at(i).transitTime;
        double detectionProbability = hits.PMTresponse.at(i).detectionProbability;

        if (!OMSimCommandArgsTable::getInstance().get<bool>("simple_PMT"))
        {
            G4double meanPE = 0;
            G4double SPEresolution = 0;
            if (OMSimHitManager::getInstance().areThereHitsInModuleSingleThread())
            {
                OMSimPMTResponse* pmtResponse = OMSimHitManager::getInstance().getSingleThreadHitsOfModule().PMTresponse.at(0).PMTResponse;
                if (pmtResponse)
                {
                    meanPE = pmtResponse->getCharge(pWavelength);
                    SPEresolution = pmtResponse->wavelengthInterpolatedValue(pmtResponse->m_gainResolutionG2Dmap, pWavelength, pWavelength);
                }
            }
            double sampledPE = -1;
            double counter = 0;
            while (sampledPE < 0)
            {
                sampledPE = G4RandGauss::shoot(meanPE, SPEresolution);
                counter++;
                if (counter > 10)
                    sampledPE = 0;
            }
            pe = sampledPE;
            
            G4double meanTransitTime = 0;
            G4double TTS = 0;
            if (OMSimHitManager::getInstance().areThereHitsInModuleSingleThread())
            {
                OMSimPMTResponse* pmtResponse = OMSimHitManager::getInstance().getSingleThreadHitsOfModule().PMTresponse.at(0).PMTResponse;
                if (pmtResponse)
                {
                    meanTransitTime = pmtResponse->getTransitTime(pWavelength);
                    TTS = pmtResponse->wavelengthInterpolatedValue(pmtResponse->m_transitTimeSpreadG2Dmap, pWavelength, pWavelength);
                }
            }
            transitTime = G4RandGauss::shoot(meanTransitTime, TTS);
        }

        dataFile << pe << "\t";
        dataFile << transitTime / ns << "\t";
        dataFile << detectionProbability << "\t";
        dataFile << pWavelength << "\t";
        dataFile << G4endl;
    }
    dataFile.close();
}