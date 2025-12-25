#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "G4ios.hh" // 添加这行以包含 G4endl 的定义

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"

ActionInitialization::ActionInitialization()
: G4VUserActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::Build() const {
    SetUserAction(new PrimaryGeneratorAction());
    auto* runAction = new RunAction();
    SetUserAction(runAction);
    auto* eventAction = new EventAction(runAction);
    SetUserAction(eventAction);
    SetUserAction(new SteppingAction(eventAction));
}

void ActionInitialization::BuildForMaster() const {
    SetUserAction(new RunAction());
}
