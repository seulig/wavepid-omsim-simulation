// File: /simulations/wavepid/OMSim_wavePID.cc
/**
 * @file 
 * @ingroup wavePID
 * @brief Main for the calculation light yields for the wavePID analysis by Steven Eulig.
 * @details The light yield is gathered by injecting monoenergetic particles at a given fixed direction and energy and position and measuring Cherenkov at the DOM.
 */
#include "OMSim.hh"
#include "OMSimwavePIDDetector.hh"
#include "OMSimTools.hh"

std::shared_ptr<spdlog::logger> g_logger;
namespace po = boost::program_options;

/**
 * @brief Add options for the user input arguments for the wavePID module
 */
void addModuleOptions(OMSim *p_simulation)
{
    po::options_description wavePIDOptions("WavePID-specific arguments");

    wavePIDOptions.add_options()
        ("world_radius,w", po::value<G4double>()->default_value(1.5), "radius of world sphere in m")
        ("waveform", po::bool_switch(), "if given, the waveform of the hits will be saved in a separate file");

    p_simulation->extendOptions(wavePIDOptions);
}

int main(int p_argCount, char *p_argumentVector[])
{
    OMSim simulation;
    addModuleOptions(&simulation);

    bool successful = simulation.handleArguments(p_argCount, p_argumentVector);
    if (!successful)
        return 0;

    // Initialize the detector
    std::unique_ptr<OMSimwavePIDDetector> detectorConstruction = std::make_unique<OMSimwavePIDDetector>();
    simulation.initialiseSimulation(detectorConstruction.get());
    detectorConstruction.release();
	
	OMSimCommandArgsTable &args = OMSimCommandArgsTable::getInstance();
	// Retrieve the 'numevents' value from your argument table
	
    G4int numevents = OMSimCommandArgsTable::getInstance().get<G4int>("numevents");

    // Visualize if enabled
    if (OMSimCommandArgsTable::getInstance().get<bool>("visual"))
        simulation.startVisualisation();

    return 0;
}