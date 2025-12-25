#pragma once
#include "G4VUserActionInitialization.hh"

class ActionInitialization : public G4VUserActionInitialization {
public:
    ActionInitialization();
    ~ActionInitialization() override;

    void Build() const override;
    void BuildForMaster() const override;
};