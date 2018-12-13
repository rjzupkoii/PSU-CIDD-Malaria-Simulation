#include "IStrategy.h"

std::map<std::string, IStrategy::StrategyType> IStrategy::StrategyTypeMap{
    {"SFT", SFT},
    {"Cycling", Cycling},
    {"AdaptiveCycling", AdaptiveCycling},
    {"MFT", MFT},
    {"MFTRebalancing", MFTRebalancing},
    {"NestedMFT", NestedMFT},
    {"MFTMultiLocation", MFTMultiLocation},
    {"NestedMFTMultiLocation", NestedMFTMultiLocation},
    {"NovelDrugSwitching", NovelDrugSwitching},
};
