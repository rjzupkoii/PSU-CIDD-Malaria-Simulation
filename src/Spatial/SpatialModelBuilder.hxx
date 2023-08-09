/*
 * SpatialModelBuilder.cpp
 * 
 * Factory pattern to return the spatial model object that should be used.
 */
#ifndef SPATIAL_SPATIALMODELBUILDER_H
#define SPATIAL_SPATIALMODELBUILDER_H

#include "Core/PropertyMacro.h"
#include "yaml-cpp/yaml.h"

#include "BarabasiSM.hxx"
#include "BurkinaFaso.hxx"
#include "MarshallSM.hxx"
#include "SpatialModel.hxx"
#include "WesolowskiSM.hxx"
#include "WesolowskiSurfaceSM.hxx"


namespace Spatial {
class SpatialModelBuilder {
  DISALLOW_COPY_AND_ASSIGN(SpatialModelBuilder)
  
  public:
    SpatialModelBuilder() = default;
    virtual ~SpatialModelBuilder() = default;
    
    static SpatialModel *Build(const std::string &name, const YAML::Node &node) {
      // Movement models based upon a mathematical model
      if (name == "Marshall") {
        return new MarshallSM(node);
      }
      if (name == "Wesolowski") {
        return new WesolowskiSM(node);
      }
      if (name == "WesolowskiSurface") {
        return new WesolowskiSurfaceSM(node);
      }

      // Potentially deprecated movement models
      if (name == "Barabasi") {
        // TODO This model can likely be deleted from the simulation, but at this time (2023-05-05) it's unclear if we
        // TODO can do so. In the mean time, make sure there is a reminder to double check the implementation.
        LOG(WARNING) << "Verify the accuracy of the underlying equation.";
        return new BarabasiSM(node);
      }

      // Country specific movement models
      if (name == "BurkinaFaso") {
        return new BurkinaFaso(node);
      }

      // Nothing matched
      LOG(WARNING) << "No match for requested spatial model, " << name;
      return nullptr;
    }
  };
}

#endif
