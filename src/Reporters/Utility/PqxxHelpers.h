/*
 * PqxxHelpers.h
 *
 * This header defines some useful helpers for use with PQXX.
 */
#ifndef PQXXHELPERS_H
#define PQXXHELPERS_H

#include <pqxx/pqxx>

namespace pqxx_db {
    // Number of times various retry events should be tried
    const int RETRY_LIMIT = 10;

    // Time to wait between trying to reconnect to the database, 10 seconds in milliseconds
    const int WAIT_TIMESPAN = 10000;

    pqxx::connection *get_connection();
}

#endif