#include "SteppingAction.hh"
#include "EventAction.hh"

#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
#include "G4TouchableHandle.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <string> // 新增：显式包含 std::string
#include "RootIO.hh" // Ensure RootIO is fully defined

/**
 * @brief 构造函数，初始化步进动作类
 * @param eventAction 指向事件动作类的指针，用于在步进过程中获取和设置事件相关信息
 */
SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(), fEventAction(eventAction) {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto track = step->GetTrack();
    auto pre   = step->GetPreStepPoint();
    auto post  = step->GetPostStepPoint();

    int eventID  = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    int trackID  = track->GetTrackID();
    int stepNo   = track->GetCurrentStepNumber();
    int pdg      = track->GetDefinition()->GetPDGEncoding();

    auto pos     = pre->GetPosition();
    auto mom     = pre->GetMomentum();
    float x      = pos.x() / CLHEP::mm;
    float y      = pos.y() / CLHEP::mm;
    float z      = pos.z() / CLHEP::mm;
    float px     = mom.x() / CLHEP::MeV;
    float py     = mom.y() / CLHEP::MeV;
    float pz     = mom.z() / CLHEP::MeV;

    float ekin   = pre->GetKineticEnergy() / CLHEP::MeV;
    float time   = pre->GetGlobalTime() / CLHEP::ns;
    float stepL  = step->GetStepLength() / CLHEP::mm;
    float edep   = step->GetTotalEnergyDeposit() / CLHEP::MeV;

    std::string volume   = pre->GetTouchableHandle()->GetVolume() ? pre->GetTouchableHandle()->GetVolume()->GetName() : "";
    std::string material = pre->GetMaterial() ? pre->GetMaterial()->GetName() : "";
    std::string process;
    if (post && post->GetProcessDefinedStep()) process = post->GetProcessDefinedStep()->GetProcessName();

    // 新增：仅在非空气材料中进行深度分布累加
    double z_cm = z / CLHEP::cm;
    if (!material.empty() && material != "G4_AIR") {
        if (z_cm > RootIO::kZMin && z_cm < RootIO::kZMax) {
            double rel = (z_cm - RootIO::kZMin) / RootIO::kBinWidth;
            int bin = static_cast<int>(rel);
            if (bin == RootIO::kNBins) bin = RootIO::kNBins - 1;
            if (fEventAction && edep > 0.) {
                fEventAction->AddBinDeposit(bin, edep * CLHEP::MeV);
            }
        }
    }

    if (auto io = fEventAction ? fEventAction->GetRootIO() : nullptr) {
        io->AddStep(eventID, trackID, stepNo, pdg,
                    x, y, z, px, py, pz,
                    ekin, time, stepL,
                    volume, material, process,
                    edep);
    }

    // 新增：仅在非空气材料中进行事件级能量与加权位置累加
    if (edep > 0. && fEventAction && !material.empty() && material != "G4_AIR") {
        auto midPos = (pre->GetPosition() + post->GetPosition()) * 0.5;
        fEventAction->AddEnergyDeposit(edep * CLHEP::MeV);
        fEventAction->AddPosition(midPos, edep * CLHEP::MeV);
        fEventAction->AddWeightSum(edep * CLHEP::MeV);
    }
}