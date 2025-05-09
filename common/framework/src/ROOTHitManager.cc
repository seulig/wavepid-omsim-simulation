// ROOTHitManager.cc

#include "ROOTHitManager.hh"
#include "OMSimLogger.hh"

ROOTHitManager::ROOTHitManager(const std::string& filename) {
    log_trace("Creating ROOT file and tree: {}", filename);
    fFile = TFile::Open(filename.c_str(), "RECREATE");
    if (!fFile || fFile->IsZombie())
    {
        log_error("Failed to create ROOT file: {}", filename);
        throw std::runtime_error("Failed to create ROOT file: " + filename);
    }
    fTree = new TTree("PhotonHits", "Photon Hit Information");

    // Initialize branches
    fTree->Branch("eventID", &fPhotonInfo.eventID, "eventID/I");
    fTree->Branch("hitTime", &fPhotonInfo.hitTime, "hitTime/D");
    fTree->Branch("entryTime", &fPhotonInfo.entryTime, "entryTime/D");
    fTree->Branch("flightTime", &fPhotonInfo.flightTime, "flightTime/D");
    fTree->Branch("pathLenght", &fPhotonInfo.pathLenght, "pathLenght/D");
    fTree->Branch("generationDetectionDistance", &fPhotonInfo.generationDetectionDistance, "generationDetectionDistance/D");
    fTree->Branch("energy", &fPhotonInfo.energy, "energy/D");
    fTree->Branch("PMTnr", &fPhotonInfo.PMTnr, "PMTnr/I");
    fTree->Branch("dir_x", &fPhotonInfo.dir_x, "dir_x/D");
    fTree->Branch("dir_y", &fPhotonInfo.dir_y, "dir_y/D");
    fTree->Branch("dir_z", &fPhotonInfo.dir_z, "dir_z/D");
    fTree->Branch("localPos_x", &fPhotonInfo.localPos_x, "localPos_x/D");
    fTree->Branch("localPos_y", &fPhotonInfo.localPos_y, "localPos_y/D");
    fTree->Branch("localPos_z", &fPhotonInfo.localPos_z, "localPos_z/D");
    fTree->Branch("globalPos_x", &fPhotonInfo.globalPos_x, "globalPos_x/D");
    fTree->Branch("globalPos_y", &fPhotonInfo.globalPos_y, "globalPos_y/D");
    fTree->Branch("globalPos_z", &fPhotonInfo.globalPos_z, "globalPos_z/D");
    fTree->Branch("deltaPos_x", &fPhotonInfo.deltaPos_x, "deltaPos_x/D");
    fTree->Branch("deltaPos_y", &fPhotonInfo.deltaPos_y, "deltaPos_y/D");
    fTree->Branch("deltaPos_z", &fPhotonInfo.deltaPos_z, "deltaPos_z/D");
    fTree->Branch("wavelength", &fPhotonInfo.wavelength, "wavelength/D");
    fTree->Branch("parentID", &fPhotonInfo.parentID, "parentID/I");
    fTree->Branch("parentType", &fPhotonInfo.parentType);    // TString handled correctly
    fTree->Branch("photonOrigin", &fPhotonInfo.photonOrigin);  // TString handled correctly
    fTree->Branch("parentProcess", &fPhotonInfo.parentProcess); // Added this line
    
    fTree->Branch("nPhotoelectrons", &fPhotonInfo.nPhotoelectrons, "nPhotoelectrons/I");

}

ROOTHitManager::~ROOTHitManager() {
    if (fFile && !fFile->IsZombie())
    {
        log_trace("Writing and closing ROOT file");
        fFile->cd();
        fTree->Write();
        fFile->Close();
    }
    delete fFile;
    fFile = nullptr;
    // Remove manual deletion of fTree
    // delete fTree;
    fTree = nullptr;
    log_trace("ROOT file and tree closed");
}

void ROOTHitManager::FillPhotonInfo(const PhotonInfoROOT& info) {
    fPhotonInfo = info;
    fTree->Fill();
}
