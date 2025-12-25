#include "RunAction.hh"
#include "RootIO.hh"
#include "G4Run.hh"
#include "G4Threading.hh"
#include "G4RunManager.hh"

RunAction::RunAction()
: G4UserRunAction()
{
    fRootIO = new RootIO();
}

RunAction::~RunAction()
{
    delete fRootIO;
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    if (G4Threading::IsMasterThread()) {
        auto* rm = G4RunManager::GetRunManager();
        G4cout << "Run " << run->GetRunID()
               << " start, events planned: "
               << (rm ? rm->GetNumberOfEventsToBeProcessed() : 0) << G4endl;
    }

    fRootIO->OpenFile("output.root");
    fRootIO->SetRunID(run->GetRunID());
    fRootIO->CreateStepTree();
    fRootIO->CreateEventTree();
    fRootIO->CreateDepthTrees(); // 新增：深度分布树
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    if (G4Threading::IsMasterThread()) {
        G4cout << "Run " << run->GetRunID()
               << " end, events processed: " << run->GetNumberOfEvent() << G4endl;
    }

    fRootIO->FillDepthMean(run->GetNumberOfEvent()); // 新增：写入本 run 的深度均值
    fRootIO->CloseFile();
}
