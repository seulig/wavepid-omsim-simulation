// OMSimActionInitialization.hh
#pragma once

#include "G4VUserActionInitialization.hh"

class OMSimTrackingAction;

class OMSimActionInitialization : public G4VUserActionInitialization {
public:
    OMSimActionInitialization();
    virtual ~OMSimActionInitialization();

    virtual void BuildForMaster() const override;
    virtual void Build() const override;
};
