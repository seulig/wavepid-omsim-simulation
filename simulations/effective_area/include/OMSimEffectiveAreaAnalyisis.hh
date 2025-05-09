// File: /simulations/effective_area/include/OMSimEffectiveAreaAnalyisis.hh
#pragma once

#include "OMSimPMTResponse.hh"
#include "OMSimHitManager.hh"

#include <G4ThreeVector.hh>
#include <fstream>
#include <iomanip>
#include "Randomize.hh"

/**
 * @brief Struct to hold results of effective area calculations.
 */
struct effectiveAreaResult
{
    double EA;      ///< Effective area.
    double EAError; ///< Uncertainty of effective area.
};

/**
 * @class OMSimEffectiveAreaAnalyisis
 * @brief Responsible for calculating the effective area of optical hits and saving the results.
 * @ingroup EffectiveArea
 */
class OMSimEffectiveAreaAnalyisis
{
public:
    OMSimEffectiveAreaAnalyisis(){};
    ~OMSimEffectiveAreaAnalyisis(){};

    template <typename... Args>
    void writeScan(Args... p_args)
    {
        std::vector<double> hits = OMSimHitManager::getInstance().countMergedHits(0, true);

        std::fstream dataFile;
        dataFile.open(m_outputFileName.c_str(), std::ios::out | std::ios::app);

        // Write all arguments to the file
        ((dataFile << p_args << "\t"), ...);

        double weightedTotal = 0;
        for (const auto &hit : hits)
        {
            dataFile << hit << "\t";
            weightedTotal = hit;
        }

        hits = OMSimHitManager::getInstance().countMergedHits(); //unweighted
        double totalHits = 0;
        for (const auto &hit : hits)
        {
            totalHits = hit; 
        }

        effectiveAreaResult effectiveArea = calculateEffectiveArea(weightedTotal, totalHits);
        dataFile << effectiveArea.EA << "\t" << effectiveArea.EAError << "\t";
        dataFile << G4endl;
        dataFile.close();
    }

    template <typename... Args>
    void writeHeader(Args... p_args)
    {
        std::fstream dataFile;
        dataFile.open(m_outputFileName.c_str(), std::ios::out | std::ios::app);
        dataFile << "# ";
        ((dataFile << p_args << "\t"), ...);
        dataFile << "hits[1perPMT]"
              << "\t"
              << "total_hits"
              << "\t"
              << "EA_Total(cm^2)"
              << "\t"
              << "EA_Total_error(cm^2)"
              << "\t" << G4endl;
        dataFile.close();
    }
    void writeHitInformation(double pWavelength);

    effectiveAreaResult calculateEffectiveArea(double weightedTotal, double countTotal);
    G4String m_outputFileName;
};