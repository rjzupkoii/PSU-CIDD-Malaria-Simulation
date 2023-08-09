/*
 * DbLoader.h
 * 
 * This class defines various methods that can be used to propagate the database
 * for the simulation for the first time. 
 */
#ifndef DBLOADER_H
#define DBLOADER_H

#include <fmt/format.h>
#include <pqxx/pqxx>
#include <string>

#include "Model.h"
#include "Core/Config/Config.h"

class DbLoader {
    public:
        // Load the genotypes into the database, assume that these are the same for ALL configurations
        static bool load_genotypes(const std::string& configuration) {
            const std::string INSERT_GENOTYPE = "INSERT INTO sim.Genotype (id, name) VALUES ({}, '{}');";

            try {
                // Load the configuration to grab the connection string
                auto* config = new Config();
                config->read_from_file(configuration);

                // Prepare the bulk query
                std::string query = "DELETE FROM sim.Genotype;";
                for (auto id = 0ul; id < config->number_of_parasite_types(); id++) {
                    auto genotype = (*config->genotype_db())[id];
                    query.append(fmt::format(INSERT_GENOTYPE, id, genotype->to_string(config)));
                }

                // Insert the data
                pqxx::connection conn(config->connection_string());
                pqxx::work db(conn);
                db.exec(query);
                db.commit();

                // Report the good news
                return true;

            } catch (const std::exception &ex) {
                std::cerr << __FUNCTION__ << "-" << ex.what() << std::endl;
                return false;
            }
        }
};

#endif