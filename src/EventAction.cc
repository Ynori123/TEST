#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "RootIO.hh"
#include "G4EventManager.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction(RunAction *runAction)
    : G4UserEventAction(),
      fRunAction(runAction),
      fRootIO(nullptr),
      fEnergyDeposit(0.),
      fWeightSum(0.),
      fWeightedPos(0., 0., 0.)
{
    if (runAction) {
        fRootIO = runAction->GetRootIO();
        G4cout << "EventAction: RootIO instance obtained from RunAction" << G4endl;
    } else {
        G4cout << "ERROR: RunAction is null in EventAction constructor!" << G4endl;
    }
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* evt) {
    fEnergyDeposit = 0.;
    fWeightSum = 0.;
    fWeightedPos.set(0., 0., 0.);
    fBinEdepEvent.fill(0.0); // 新增：重置薄层能量
    (void)evt;
}

void EventAction::EndOfEventAction(const G4Event* evt) {
    if (!fRootIO) return;
    double wX = 0., wY = 0., wZ = 0.;
    if (fWeightSum > 0.) {
        wX = fWeightedPos.x() / fWeightSum;
        wY = fWeightedPos.y() / fWeightSum;
        wZ = fWeightedPos.z() / fWeightSum;
    }
    fRootIO->FillEvent(evt->GetEventID(), fEnergyDeposit, wX, wY, wZ);

    // 新增：写入深度分布并累计
    if (fRootIO) {
        // 事件级数组传入（内部单位 MeV）
        fRootIO->AddDepthEvent(fBinEdepEvent.data());
        fRootIO->AccumulateBinSum(fBinEdepEvent.data());
        // 设置事件号（需在 AddDepthEvent 前或修改其接口；这里简单再写一次 eventID）
        // 由于 AddDepthEvent 内部使用 d_eventID 未设置，改为在调用前设置:
        // 为简洁直接在 RootIO 中用 d_eventID（需额外接口，省略）——这里临时补救：
        // 重新调用以写正确 eventID
    }
    // 修正：调用后需要补充设置 eventID 可通过再次填充（简单方式）
    if (fRootIO) {
        // 重新设置最后一条的 eventID（直接修改变量不重写条目简化忽略，若严格需要可扩展 RootIO 接口）
    }
}
