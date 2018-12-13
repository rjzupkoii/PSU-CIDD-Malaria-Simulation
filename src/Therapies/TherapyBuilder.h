/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TherapyBuilder.h
 * Author: Merlin
 *
 * Created on August 23, 2017, 2:43 PM
 */

#ifndef THERAPYBUILDER_H
#define THERAPYBUILDER_H

#include <yaml-cpp/yaml.h>

class Therapy;

class TherapyBuilder {
 public:
  TherapyBuilder();

  TherapyBuilder(const TherapyBuilder &orig) = default;

  virtual ~TherapyBuilder();

  static Therapy *build(const YAML::Node &ns, const int &t_id);

 private:

};

#endif /* THERAPYBUILDER_H */

