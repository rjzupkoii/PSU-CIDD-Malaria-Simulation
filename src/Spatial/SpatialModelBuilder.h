//
// Created by Nguyen Tran on 1/28/2018.
//

#ifndef SPATIAL_SPATIALMODELBUILDER_H
#define SPATIAL_SPATIALMODELBUILDER_H

#include "Core/PropertyMacro.h"
#include "SpatialModel.h"
#include "yaml-cpp/yaml.h"

namespace Spatial {
class SpatialModelBuilder {
 DISALLOW_COPY_AND_ASSIGN(SpatialModelBuilder)

 public:
  SpatialModelBuilder();

  virtual ~SpatialModelBuilder();

  static SpatialModel *Build(const std::string &name, const YAML::Node &node);

};
}

#endif //SPATIAL_SPATIALMODELBUILDER_H
