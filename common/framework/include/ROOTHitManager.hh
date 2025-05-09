// ROOTHitManager.hh

#pragma once

#include "TFile.h"
#include "TTree.h"
#include "G4Types.hh"
#include "G4ThreeVector.hh"
#include "TString.h" // Include TString

struct PhotonInfoROOT {
    Int_t    eventID;
    Double_t hitTime;
    Double_t flightTime;
    Double_t entryTime;
    Double_t pathLenght;
    Double_t generationDetectionDistance;
    Double_t energy;
    Int_t    PMTnr;
    Double_t dir_x;
    Double_t dir_y;
    Double_t dir_z;
    Double_t localPos_x;
    Double_t localPos_y;
    Double_t localPos_z;
    Double_t globalPos_x;
    Double_t globalPos_y;
    Double_t globalPos_z;
    Double_t deltaPos_x;
    Double_t deltaPos_y;
    Double_t deltaPos_z;
    Double_t wavelength;
    Int_t    parentID;
    TString parentType;    // Changed from std::string to TString
    TString photonOrigin;  // Changed from std::string to TString
    TString parentProcess; // Added this line
    Int_t nPhotoelectrons;
};


class ROOTHitManager {
public:
    ROOTHitManager(const std::string& filename);
    ~ROOTHitManager();

    void FillPhotonInfo(const PhotonInfoROOT& info);

private:
    TFile* fFile;
    TTree* fTree;
    PhotonInfoROOT fPhotonInfo;
};
