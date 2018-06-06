/* 
 * File:   InitialParasiteInfo.h
 * Author: nguyentran
 *
 * Created on March 28, 2013, 9:56 AM
 */

#ifndef INITIALPARASITEINFO_H
#define	INITIALPARASITEINFO_H

#include <yaml-cpp/yaml.h>
#include <boost/foreach.hpp>

//struct ParasiteInfo {
//    int parasite_type_id;
//    double prevalence;
//};
//
//typedef std::vector<ParasiteInfo> ParasiteInfoVector;
//
//struct InitialParasiteInfo {
//    int location_id;
//    ParasiteInfoVector v_parasite_info;
//};
//
//typedef std::vector<InitialParasiteInfo> InitialParasiteInfoVector;
//
//namespace YAML {
//
//    template<>
//    struct convert<ParasiteInfo> {
//
//        static Node encode(const ParasiteInfo& rhs) {
//            Node node;
//            node["parasite_type_id"] = rhs.parasite_type_id;
//            node["prevalence"] = rhs.prevalence;
//            return node;
//        }
//
//        static bool decode(const Node& node, ParasiteInfo& rhs) {
//            rhs.parasite_type_id = node["parasite_type_id"].as<int>();
//            rhs.prevalence = node["prevalence"].as<double>();
//            return true;
//        }
//    };
//
//    template<>
//    struct convert<ParasiteInfoVector> {
//
//        static Node encode(const ParasiteInfoVector& rhs) {
//            Node node;
//            for(const ParasiteInfo& p_info :  rhs) {
//                node.push_back(p_info);
//            }
//            return node;
//        }
//
//        static bool decode(const Node& node, ParasiteInfoVector& rhs) {
//
//            for(const YAML::Node &item :  node) {
//                rhs.push_back(item.as<ParasiteInfo>());
//            }
//            return true;
//        }
//    };
//
//    template<>
//    struct convert<InitialParasiteInfo> {
//
//        static Node encode(const InitialParasiteInfo& rhs) {
//            Node node;
//            node["location_id"] = rhs.location_id;
//            node["parasite_info"] = rhs.v_parasite_info;
//            return node;
//        }
//
//        static bool decode(const Node& node, InitialParasiteInfo& rhs) {
//            rhs.location_id = node["location_id"].as<int>();
//            rhs.v_parasite_info = node["parasite_info"].as<ParasiteInfoVector>();
//            return true;
//        }
//    };
//    
//    template<>
//    struct convert<InitialParasiteInfoVector> {
//
//        static Node encode(const InitialParasiteInfoVector& rhs) {
//            Node node;
//            for(const InitialParasiteInfo& p_info :  rhs) {
//                node.push_back(p_info);
//            }
//            return node;
//        }
//
//        static bool decode(const Node& node, InitialParasiteInfoVector& rhs) {
//
//            for(const YAML::Node &item :  node) {
//                rhs.push_back(item.as<InitialParasiteInfo>());
//            }
//            return true;
//        }
//    };
//}


#endif	/* INITIALPARASITEINFO_H */

