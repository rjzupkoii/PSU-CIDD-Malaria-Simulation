#include "IStrategy.h"

std::map<std::string, IStrategy::StrategyType> IStrategy::StrategyTypeMap{
    {"SFT", SFT},
    {"Cycling", Cycling},
    {"MFT", MFT},
    {"NestedMFT", NestedMFT},
    {"MFTMultiLocation", MFTMultiLocation},
    {"NestedMFTMultiLocation", NestedMFTMultiLocation},
};
