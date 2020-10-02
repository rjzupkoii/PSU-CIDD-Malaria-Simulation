/*
 * CellularReporter.cpp
 * 
 * Implementation of the CellularReporter class.
 */
#include "CellularReporter.h"

#include "Core/Config/Config.h"
#include "MDC/ModelDataCollector.h"
#include "Model.h"
#include "easylogging++.h"

void CellularReporter::initialize(int job_number, std::string path) {
    // Check the number of cells in the model, if it exceeds one, throw an error

    // Prepare the loggers to record the data

    // Log the headers

    // Aggregate - DaysElapsed, Population, Infected, 580Y Weighted

    // Detailed - DaysElapsed, Individual, [Genome 1 - N]

}

void CellularReporter::monthly_report() {

    // Grab the ids of the genomes that are 580Y
    
    // Iterate through all of the individuals to generate the aggregate data, track values for weighted 580Y

    // Store teh data

    // If we are in the detailed reporting phase, run that function

}

// Generate a detailed report by recording the infection status of all individuals 
// in the model. The expectation is that this function will be called by monthly_report
// during the appropriate time frame.
void CellularReporter::detailed_report() {

    // Get the ids of the genomes so we can preserve order

    // Iterate through all of the individuals

    // Since individuals don't get a unique id assigned, use the memory address instead

    // Record the genome values
    
}
