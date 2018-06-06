#include "IStrategy.h"
#include <boost/assign/list_of.hpp>

std::map<std::string, IStrategy::StrategyType> IStrategy::StrategyTypeMap = boost::assign::map_list_of("SFT", SFT)
        ("Cycling", Cycling)("AdaptiveCycling", AdaptiveCycling)("MFT", MFT)("ACTIncreaseOvertime", ACTIncreaseOvertime)
        ("NovelNonACTSwitching", NovelNonACTSwitching)("TACTSwitching", TACTSwitching)
        ("MFTRebalancing", MFTRebalancing)("NestedSwitching", NestedSwitching)
        ("MFTDifferentDistributionByLocation", MFTDifferentDistributionByLocation)
        ("NestedSwitchingDifferentDistributionByLocation", NestedSwitchingDifferentDistributionByLocation);