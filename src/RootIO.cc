#include "RootIO.hh"
#include <iostream>
#include <cstring>
#include <TROOT.h>   // 为 Form 提供声明

RootIO::RootIO() {}
RootIO::~RootIO() { CloseFile(); }

void RootIO::OpenFile(const std::string& filename) {
    if (fFile) CloseFile();
    fFile = TFile::Open(filename.c_str(), "UPDATE");
    if (!fFile || fFile->IsZombie()) {
        std::cerr << "[RootIO] Failed to open ROOT file (UPDATE): " << filename << std::endl;
        if (fFile) { fFile->Close(); delete fFile; }
        fFile = nullptr;
    }
}

void RootIO::CreateStepTree() {
    if (!fFile) return;
    fFile->cd();
    fStepTree = dynamic_cast<TTree*>(fFile->Get("stepTree"));
    if (fStepTree) { AttachStepBranches(); return; }
    fStepTree = new TTree("stepTree", "Per-step data");
    fStepTree->Branch("runID",    &s_runID,   "runID/I");
    fStepTree->Branch("eventID",  &s_eventID, "eventID/I");
    fStepTree->Branch("trackID",  &s_trackID, "trackID/I");
    fStepTree->Branch("stepNo",   &s_stepNo,  "stepNo/I");
    fStepTree->Branch("pdg",      &s_pdg,     "pdg/I");
    fStepTree->Branch("x",        &s_x,       "x/F");
    fStepTree->Branch("y",        &s_y,       "y/F");
    fStepTree->Branch("z",        &s_z,       "z/F");
    fStepTree->Branch("px",       &s_px,      "px/F");
    fStepTree->Branch("py",       &s_py,      "py/F");
    fStepTree->Branch("pz",       &s_pz,      "pz/F");
    fStepTree->Branch("ekin",     &s_ekin,    "ekin/F");
    fStepTree->Branch("time",     &s_time,    "time/F");
    fStepTree->Branch("stepLen",  &s_stepLen, "stepLen/F");
    fStepTree->Branch("edep",     &s_edep,    "edep/F");

    std::string volumeBranch = Form("volume[%d]/C", kStrMax);
    std::string materialBranch = Form("material[%d]/C", kStrMax);
    std::string processBranch = Form("process[%d]/C", kStrMax);

    fStepTree->Branch("volume",   s_volume,   volumeBranch.c_str());
    fStepTree->Branch("material", s_material, materialBranch.c_str());
    fStepTree->Branch("process",  s_process,  processBranch.c_str());
}

void RootIO::CreateEventTree() {
    if (!fFile) return;
    fFile->cd();
    fEventTree = dynamic_cast<TTree*>(fFile->Get("eventTree"));
    if (fEventTree) { AttachEventBranches(); return; }
    fEventTree = new TTree("eventTree", "Per-event summary");
    fEventTree->Branch("runID", &e_runID, "runID/I");
    fEventTree->Branch("eventID", &e_eventID, "eventID/I");
    fEventTree->Branch("totalEnergyDeposit", &e_totalEnergy, "totalEnergyDeposit/D");
    fEventTree->Branch("weightedPosX", &e_wposX, "weightedPosX/D");
    fEventTree->Branch("weightedPosY", &e_wposY, "weightedPosY/D");
    fEventTree->Branch("weightedPosZ", &e_wposZ, "weightedPosZ/D");
}

void RootIO::CreateDepthTrees() {
    if (!fFile) return;
    fFile->cd();
    fDepthEventTree = dynamic_cast<TTree*>(fFile->Get("depthEventTree"));
    if (fDepthEventTree) {
        AttachDepthEventBranches();
    } else {
        fDepthEventTree = new TTree("depthEventTree", "Per-event depth energy deposition");
        fDepthEventTree->Branch("runID",    &d_runID,   "runID/I");
        fDepthEventTree->Branch("eventID",  &d_eventID, "eventID/I");
        fDepthEventTree->Branch("binEdep",  d_binEdep,  Form("binEdep[%d]/D", kNBins));
    }
    fDepthMeanTree = dynamic_cast<TTree*>(fFile->Get("depthMeanTree"));
    if (fDepthMeanTree) {
        AttachDepthMeanBranches();
    } else {
        fDepthMeanTree = new TTree("depthMeanTree", "Per-run mean depth energy deposition");
        fDepthMeanTree->Branch("runID",    &m_runID,   "runID/I");
        fDepthMeanTree->Branch("nEvents",  &m_nEvents, "nEvents/I");
        fDepthMeanTree->Branch("meanEdep", m_binMean,  Form("meanEdep[%d]/D", kNBins));
        fDepthMeanTree->Branch("dEdz",     m_dEdz,     Form("dEdz[%d]/D", kNBins));
    }
    fBinSum.fill(0.0);
}

void RootIO::AttachStepBranches() {
    fStepTree->SetBranchAddress("runID",    &s_runID);
    fStepTree->SetBranchAddress("eventID",  &s_eventID);
    fStepTree->SetBranchAddress("trackID",  &s_trackID);
    fStepTree->SetBranchAddress("stepNo",   &s_stepNo);
    fStepTree->SetBranchAddress("pdg",      &s_pdg);
    fStepTree->SetBranchAddress("x",        &s_x);
    fStepTree->SetBranchAddress("y",        &s_y);
    fStepTree->SetBranchAddress("z",        &s_z);
    fStepTree->SetBranchAddress("px",       &s_px);
    fStepTree->SetBranchAddress("py",       &s_py);
    fStepTree->SetBranchAddress("pz",       &s_pz);
    fStepTree->SetBranchAddress("ekin",     &s_ekin);
    fStepTree->SetBranchAddress("time",     &s_time);
    fStepTree->SetBranchAddress("stepLen",  &s_stepLen);
    fStepTree->SetBranchAddress("edep",     &s_edep);
    fStepTree->SetBranchAddress("volume",   s_volume);
    fStepTree->SetBranchAddress("material", s_material);
    fStepTree->SetBranchAddress("process",  s_process);
}

void RootIO::AttachEventBranches() {
    fEventTree->SetBranchAddress("runID", &e_runID);
    fEventTree->SetBranchAddress("eventID", &e_eventID);
    fEventTree->SetBranchAddress("totalEnergyDeposit", &e_totalEnergy);
    fEventTree->SetBranchAddress("weightedPosX", &e_wposX);
    fEventTree->SetBranchAddress("weightedPosY", &e_wposY);
    fEventTree->SetBranchAddress("weightedPosZ", &e_wposZ);
}

void RootIO::AttachDepthEventBranches() {
    fDepthEventTree->SetBranchAddress("runID",   &d_runID);
    fDepthEventTree->SetBranchAddress("eventID", &d_eventID);
    fDepthEventTree->SetBranchAddress("binEdep", d_binEdep);
}

void RootIO::AttachDepthMeanBranches() {
    fDepthMeanTree->SetBranchAddress("runID",    &m_runID);
    fDepthMeanTree->SetBranchAddress("nEvents",  &m_nEvents);
    fDepthMeanTree->SetBranchAddress("meanEdep", m_binMean);
    fDepthMeanTree->SetBranchAddress("dEdz",     m_dEdz);
}

void RootIO::AddStep(int eventID, int trackID, int stepNo, int pdg,
                     float x, float y, float z,
                     float px, float py, float pz,
                     float ekin, float time, float stepLen,
                     const std::string& volume, const std::string& material, const std::string& process,
                     float edep) {
    if (!fStepTree) return;
    s_runID = fRunID;
    s_eventID = eventID;
    s_trackID = trackID;
    s_stepNo = stepNo;
    s_pdg = pdg;
    s_x = x; s_y = y; s_z = z;
    s_px = px; s_py = py; s_pz = pz;
    s_ekin = ekin; s_time = time; s_stepLen = stepLen;
    s_edep = edep;
    auto copySafe = [&](char* dst, const std::string& src){
        std::strncpy(dst, src.c_str(), kStrMax - 1);
        dst[kStrMax - 1] = '\0';
    };
    copySafe(s_volume, volume);
    copySafe(s_material, material);
    copySafe(s_process, process);
    fStepTree->Fill();
}

void RootIO::FillEvent(int eventID, double totalEnergy,
                       double wposX, double wposY, double wposZ) {
    if (!fEventTree) return;
    e_runID = fRunID;
    e_eventID = eventID;
    e_totalEnergy = totalEnergy;
    e_wposX = wposX; e_wposY = wposY; e_wposZ = wposZ;
    fEventTree->Fill();
}

void RootIO::AddDepthEvent(const double* binEdep) {
    if (!fDepthEventTree) return;
    d_runID = fRunID;
    d_eventID = e_eventID; // 使用已填充的事件号
    for (int i = 0; i < kNBins; ++i) d_binEdep[i] = binEdep[i];
    fDepthEventTree->Fill();
}

void RootIO::AccumulateBinSum(const double* binEdep) {
    for (int i = 0; i < kNBins; ++i) fBinSum[i] += binEdep[i];
}

void RootIO::FillDepthMean(int nEvents) {
    if (!fDepthMeanTree || nEvents <= 0) return;
    m_runID = fRunID;
    m_nEvents = nEvents;
    for (int i = 0; i < kNBins; ++i) {
        m_binMean[i] = fBinSum[i] / nEvents;
        m_dEdz[i] = m_binMean[i] / kBinWidth;
    }
    fDepthMeanTree->Fill();
}

void RootIO::WriteStepTree() {
    if (fFile && fStepTree) { fFile->cd(); fStepTree->Write("stepTree", TObject::kWriteDelete); }
}

void RootIO::WriteEventTree() {
    if (fFile && fEventTree) { fFile->cd(); fEventTree->Write("eventTree", TObject::kWriteDelete); }
}

static void WriteTreeIf(TFile* f, TTree* t, const char* name) {
    if (f && t) { f->cd(); t->Write(name, TObject::kWriteDelete); }
}

void RootIO::CloseFile() {
    WriteStepTree();
    WriteEventTree();
    WriteTreeIf(fFile, fDepthEventTree, "depthEventTree");
    WriteTreeIf(fFile, fDepthMeanTree,  "depthMeanTree");
    if (fFile) {
        fFile->Purge();
        fFile->Write();
        fFile->Close();
        delete fFile;
        fFile = nullptr;
        fStepTree = nullptr;
        fEventTree = nullptr;
        fDepthEventTree = nullptr;
        fDepthMeanTree  = nullptr;
    }
}