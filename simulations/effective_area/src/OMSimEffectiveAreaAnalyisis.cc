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

void OMSimEffectiveAreaAnalyisis::writeHitInformation(double pWavelength)
{
    // This method is intentionally left empty as the data is now handled by ROOT
    // and the HitManager is not used directly here anymore.
    log_trace("Writing hit information to ROOT file");
}