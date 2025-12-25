#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
class G4Run;
class RootIO;

class RunAction : public G4UserRunAction {
public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;

    RootIO* GetRootIO() { return fRootIO; }

private:
    RootIO* fRootIO;
};

#endif // RUNACTION_HH
