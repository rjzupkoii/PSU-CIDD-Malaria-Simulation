/*
 * TherapyBuilder.h
 *
 * Define the factory pattern to create the therapy objects.
 */
#ifndef THERAPYBUILDER_H
#define THERAPYBUILDER_H

#include <yaml-cpp/yaml.h>

class Therapy;

class TherapyBuilder {

 public:
  TherapyBuilder() = default;
  TherapyBuilder(const TherapyBuilder &orig) = default;
  virtual ~TherapyBuilder() = default;

  static Therapy *build(const YAML::Node &ns, const int &t_id);

 private:
    static Therapy *create_simple(const YAML::Node &ns, const int &t_id);
    static Therapy *create_complex(const YAML::Node &ns, const int &t_id);
};

#endif

