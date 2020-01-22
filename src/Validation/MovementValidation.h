/*
 * MovementValidation.h
 * 
 * Defines various methods needed for movement validation.
 */
#ifndef MOVEMENTVALIDATION_H
#define MOVEMENTVALIDATION_H

#include "Core/PropertyMacro.h"
#include "Reporters/MovementReporter.h"

class MovementValidation {

    PROPERTY_REF(bool, cell_movement)
    PROPERTY_REF(bool, district_movement)
    PROPERTY_REF(bool, individual_movement)

    private:
        MovementValidation() { }
        ~MovementValidation() { }

        int replicate;
        MovementReporter* reporter;
        std::string query;

    public:
        // Not supported by singleton.
        MovementValidation(MovementValidation const&) = delete;

        // Not supported by singleton.
        void operator=(MovementValidation const&) = delete;

        // Get a reference to the spatial object.
        static MovementValidation& get_instance() {
            static MovementValidation instance;
            return instance;
        }

        // Add the movement to the record
        static void add_move(int individual, int source, int destination);

        // Set the reporter to use
        void set_reporter(MovementReporter* reporter) { this->reporter = reporter; }

        // Write the movement data that is generated after model initiation.
        static void write_movement_data();
};

#endif