// File: /simulations/effective_area/OMSim_effective_area.cc
/**
 * @file 
 * @ingroup EffectiveArea
 * @brief Main for the calculation of effective areas.
 * @details The effective area of a module is calculated simulating a plane wave from a certain direction.
 */
#include "OMSim.hh"
#include "OMSimAngularScan.hh"
#include "OMSimEffectiveAreaAnalyisis.hh"
#include "OMSimEffectiveAreaDetector.hh"
#include "OMSimTools.hh"

std::shared_ptr<spdlog::logger> g_logger;
namespace po = boost::program_options;

void runEffectiveAreaSimulation()
{
    OMSimEffectiveAreaAnalyisis analysisManager;
    OMSimCommandArgsTable &args = OMSimCommandArgsTable::getInstance();
    OMSimHitManager &hitManager = OMSimHitManager::getInstance();
    //AngularScan *scanner = new AngularScan(args.get<G4double>("radius"), args.get<G4double>("distance"), args.get<G4double>("wavelength"));

    //analysisManager.m_outputFileName = args.get<std::string>("output_file") + ".dat";

}

/**
 * @brief Add options for the user input arguments for the effective area module
 */
void addModuleOptions(OMSim *p_simulation)
{
    po::options_description effectiveAreaOptions("Effective area specific arguments");

    effectiveAreaOptions.add_options()
        ("world_radius,w", po::value<G4double>()->default_value(1.5), "radius of world sphere in m")
        ("radius,r", po::value<G4double>()->default_value(300.0), "plane wave radius in mm")
        ("distance,d", po::value<G4double>()->default_value(2000), "plane wave distance from origin, in mm")
        ("theta,t", po::value<G4double>()->default_value(0.0), "theta (= zenith) in deg")
        ("phi,f", po::value<G4double>()->default_value(0.0), "phi (= azimuth) in deg")
        ("wavelength,l", po::value<G4double>()->default_value(400.0), "wavelength of incoming light in nm")
        ("angles_file,i", po::value<std::string>(), "The input angle pairs file to be scanned. The file should contain two columns, the first column with the theta (zenith) and the second with phi (azimuth) in degrees.")
        ("no_header", po::bool_switch(), "if given, the header of the output file will not be written")
        ("waveform", po::bool_switch(), "if given, the waveform of the hits will be saved in a separate file");

    p_simulation->extendOptions(effectiveAreaOptions);
}

int main(int p_argCount, char *p_argumentVector[])
{
    OMSim simulation;
    addModuleOptions(&simulation);

    bool successful = simulation.handleArguments(p_argCount, p_argumentVector);
    if (!successful)
        return 0;

    // Initialize the detector
    std::unique_ptr<OMSimEffectiveAreaDetector> detectorConstruction = std::make_unique<OMSimEffectiveAreaDetector>();
    simulation.initialiseSimulation(detectorConstruction.get());
    detectorConstruction.release();

    // Run the simulation
    runEffectiveAreaSimulation();
	
	
	OMSimCommandArgsTable &args = OMSimCommandArgsTable::getInstance();
	// Retrieve the 'numevents' value from your argument table
	
    G4int numevents = OMSimCommandArgsTable::getInstance().get<G4int>("numevents");
	 // Specify the path to the file you want to create/write.
    // Note: This path must exist on your system; otherwise, you'll need to create the directory first.
    std::string filename = "/Users/steveneulig/geant4_projects/IceCubeSimulation/OMSim/common/data/vis/vis.mac";

    // Open the file in output mode (this will create the file if it doesn't exist,
    // or overwrite it if it does exist).
    std::ofstream outFile(filename);
    

    // Write the content to the file.
    outFile << "/control/verbose 0\n";
    outFile << "/control/saveHistory\n";
    outFile << "/run/verbose 0\n";
    outFile << "/run/beamOn " << numevents << "\n";
    outFile << "exit\n";

    // Close the file
    outFile.close();


    // Visualize if enabled
    if (OMSimCommandArgsTable::getInstance().get<bool>("visual"))
        simulation.startVisualisation();

    return 0;
}