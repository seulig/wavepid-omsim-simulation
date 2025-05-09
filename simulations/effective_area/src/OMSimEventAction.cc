// File: /simulations/effective_area/src/OMSimEventAction.cc
#include "OMSimEventAction.hh"
#include <G4RunManager.hh>
#include "OMSimCommandArgsTable.hh"

void OMSimEventAction::BeginOfEventAction(const G4Event* p_event)
{
}

void OMSimEventAction::EndOfEventAction(const G4Event* p_event)
{
    if (OMSimCommandArgsTable::getInstance().get<bool>("waveform"))
    {
        analysisManager.writeHitInformation(OMSimCommandArgsTable::getInstance().get<G4double>("wavelength"));
    }
}