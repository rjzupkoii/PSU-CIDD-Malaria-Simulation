//
// Created by Nguyen Tran on 1/28/2018.
//

#include "SpatialModelBuilder.h"
#include "GeneralGravitySM.h"
#include "BarabasiSM.h"
#include "WesolowskiSM.h"

namespace Spatial {
SpatialModelBuilder::SpatialModelBuilder() = default;

SpatialModelBuilder::~SpatialModelBuilder() = default;

SpatialModel *SpatialModelBuilder::Build(const std::string &name, const YAML::Node &node) {

  if (name=="Gravity") {

    return new GeneralGravitySM();
  }
  if (name=="Barabasi") {
    return new BarabasiSM(node);
  }

  if (name=="Wesolowski") {
    return new WesolowskiSM(node);
  }

  return nullptr;
}

}