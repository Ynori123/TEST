#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include <array>
#include "RootIO.hh" // Include the full definition of RootIO

class RunAction;
class RootIO;

class EventAction : public G4UserEventAction {
public:
    EventAction(RunAction* runAction);
    ~EventAction() override;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEnergyDeposit(G4double edep) { fEnergyDeposit += edep; }
    void AddPosition(const G4ThreeVector& pos, G4double weight) { fWeightedPos += pos * weight; }
    void AddWeightSum(G4double w) { fWeightSum += w; }
    void AddBinDeposit(int bin, G4double edep) { if (bin>=0 && bin<RootIO::kNBins) fBinEdepEvent[bin] += edep; }

    RootIO* GetRootIO() const { return fRootIO; }
    const std::array<G4double, RootIO::kNBins>& GetBinArray() const { return fBinEdepEvent; }

private:
    RunAction*    fRunAction;
    RootIO*       fRootIO;
    G4double      fEnergyDeposit;
    G4double      fWeightSum;
    G4ThreeVector fWeightedPos;
    std::array<G4double, RootIO::kNBins> fBinEdepEvent{}; // 每事件各薄层能量（MeV 单位）
};

#endif // EVENTACTION_HH
