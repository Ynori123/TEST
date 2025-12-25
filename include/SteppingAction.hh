#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

class EventAction;

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction() override = default;

    virtual void UserSteppingAction(const G4Step* step) override;

private:
    EventAction* fEventAction;
};

#endif // SteppingAction_h