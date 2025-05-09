// File: /simulations/wavepid/include/OMSimwavePIDDetector.hh
#pragma once

#include "OMSimDetectorConstruction.hh"

/**
 * @class OMSimwavePIDDetector
 * @brief Class for detector construction in the wavePID simulation.
 * @ingroup wavePID
 */
class OMSimwavePIDDetector : public OMSimDetectorConstruction
{
public:
    OMSimwavePIDDetector() : OMSimDetectorConstruction(){};
    ~OMSimwavePIDDetector(){};

private:
    void constructWorld();
    void constructDetector();
};