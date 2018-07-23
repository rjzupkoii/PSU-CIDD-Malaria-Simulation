#include "IStrategy.h"

std::map<std::string, IStrategy::StrategyType> IStrategy::StrategyTypeMap{
    {"SFT", SFT},
    {"Cycling", Cycling},
    {"AdaptiveCycling", AdaptiveCycling},
    {"MFT", MFT},
    {"ACTIncreaseOvertime", ACTIncreaseOvertime},
    {"MFTRebalancing", MFTRebalancing},
    {"NestedMFT", NestedMFT},
    {"MFTDifferentDistributionByLocation", MFTDifferentDistributionByLocation},
    {"NestedMFTDifferentDistributionByLocation", NestedMFTDifferentDistributionByLocation},
};
