// File: /simulations/wavepid/src/OMSimwavePIDDetector.cc
#include "OMSimwavePIDDetector.hh"
#include "OMSimPDOM.hh"
#include "OMSimLOM16.hh"
#include "OMSimLOM18.hh"
#include "OMSimDEGG.hh"
#include "OMSimMDOM.hh"
#include "OMSimCommandArgsTable.hh"
#include "OMSimHitManager.hh"
#include <G4Orb.hh>
#include "OMSimSensitiveDetector.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

/**
 * @brief Constructs the world volume (sphere).
 */
void OMSimwavePIDDetector::constructWorld()
{
    // Use the world radius provided via command-line arguments
    G4double dom_event_spacing = OMSimCommandArgsTable::getInstance().get<G4double>("dom_event_spacing");
    G4double worldRadius = OMSimCommandArgsTable::getInstance().get<G4double>("world_radius");
    m_worldSolid = new G4Orb("World", dom_event_spacing*1.8 * m);

    // Configure material properties for water
    G4Material* water = G4Material::GetMaterial("IceCubeICE_SPICE");
    if (!water->GetMaterialPropertiesTable())
    {
        G4MaterialPropertiesTable* waterMPT = new G4MaterialPropertiesTable();
        const G4int nEntries = 2;
        G4double photonEnergies[nEntries] = {2.0 * eV, 3.5 * eV};
        G4double refractiveIndices[nEntries] = {1.33, 1.33}; // Refractive index of water
        waterMPT->AddProperty("RINDEX", photonEnergies, refractiveIndices, nEntries);
        water->SetMaterialPropertiesTable(waterMPT);
    }

    m_worldLogical = new G4LogicalVolume(m_worldSolid, water, "World_log", 0, 0, 0);
    m_worldPhysical = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), m_worldLogical, "World_phys", 0, false, 0);

    // Set visualization attributes for the world
    G4VisAttributes *worldVis = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.2)); // Transparent blue for water
    worldVis->SetVisibility(true);
    worldVis->SetForceSolid(true);
    m_worldLogical->SetVisAttributes(worldVis);
}

/**
 * @brief Constructs the selected detector from the command line argument and returns the physical world volume.
 * @return Pointer to the physical world volume
 */
void OMSimwavePIDDetector::constructDetector()
{
    OMSimHitManager &hitManager = OMSimHitManager::getInstance();

    // Get detector type and whether to place the harness
    bool placeHarness = OMSimCommandArgsTable::getInstance().get<bool>("place_harness");
    OMSimOpticalModule *opticalModule = nullptr;

    // Select the detector type
    switch (OMSimCommandArgsTable::getInstance().get<G4int>("detector_type"))
    {
    case 0:
        log_critical("No custom detector implemented!");
        break;
    case 3:
        log_info("Constructing pDOM");
        opticalModule = new pDOM(placeHarness);
        break;
    default:
        log_critical("Invalid detector type or unsupported detector type!");
        return;
    }

    // Place and configure the detector
    if (opticalModule)
    {

        G4double dom_event_spacing = OMSimCommandArgsTable::getInstance().get<G4double>("dom_event_spacing");
        opticalModule->placeIt(G4ThreeVector(0.0*m, dom_event_spacing*m, 0.0*m), G4RotationMatrix(), m_worldLogical, "");
        //opticalModule->placeIt(G4ThreeVector(0, 0, 0), G4RotationMatrix(), m_worldLogical, "");
        opticalModule->configureSensitiveVolume(this);
        
        // Log the placement of the optical module
        log_debug("Optical module placed in the world volume.");
    
        // Set visualization attributes for the detector
        G4LogicalVolume *domLogical = m_worldLogical->GetDaughter(0)->GetLogicalVolume();
        if (domLogical)
        {
            G4VisAttributes *domVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // Red color for DOM
            domVis->SetVisibility(true);
            domVis->SetForceSolid(true);
            domLogical->SetVisAttributes(domVis);

            log_debug("Visualization attributes set for the optical module logical volume.");
        }
        else
        {
            log_error("Failed to retrieve the logical volume of the optical module.");
        }
    
    }
}