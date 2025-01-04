// File: /simulations/effective_area/include/OMSimEffectiveAreaDetector.hh
#pragma once

#include "OMSimDetectorConstruction.hh"

/**
 * @class OMSimEffectiveAreaDetector
 * @brief Class for detector construction in the effective area simulation.
 * @ingroup EffectiveArea
 */
class OMSimEffectiveAreaDetector : public OMSimDetectorConstruction
{
public:
    OMSimEffectiveAreaDetector() : OMSimDetectorConstruction(){};
    ~OMSimEffectiveAreaDetector(){};

private:
    void constructWorld();
    void constructDetector();
};