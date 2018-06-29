#include "IStrategy.h"

std::map<std::string, IStrategy::StrategyType> IStrategy::StrategyTypeMap{{"SFT",                                            SFT},
                                                                          {"Cycling",                                        Cycling},
                                                                          {"AdaptiveCycling",                                AdaptiveCycling},
                                                                          {"MFT",                                            MFT},
                                                                          {"ACTIncreaseOvertime",                            ACTIncreaseOvertime},
                                                                          {"NovelNonACTSwitching",                           NovelNonACTSwitching},
                                                                          {"TACTSwitching",                                  TACTSwitching},
                                                                          {"MFTRebalancing",                                 MFTRebalancing},
                                                                          {"NestedSwitching",                                NestedSwitching},
                                                                          {"MFTDifferentDistributionByLocation",             MFTDifferentDistributionByLocation},
                                                                          {"NestedSwitchingDifferentDistributionByLocation", NestedSwitchingDifferentDistributionByLocation},};