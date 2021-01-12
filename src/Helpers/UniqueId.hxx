/*
 * UniqueId.hxx
 * 
 * Define a thread-safe singleton pattern class that allows for unique identifiers
 * to be returned for the lifecycle of a single simulation. This is useful for 
 * some of the deeper model identification and validation to be done.
 */
#ifndef UNIQUEID_HXX
#define UNIQUEID_HXX

typedef unsigned long ul_uid;

class UniqueId {
    private:
        ul_uid current_id;

        // Constructor
        UniqueId() { current_id = 0; }

        // Deconstructor
        ~UniqueId() = default;


    public:
        // Not supported by singleton
        UniqueId(UniqueId const&) = delete;

        // Not suported by singleton
        void operator=(UniqueId const&) = delete;

        // Get a reference to the object
        static UniqueId& get_instance() {
            static UniqueId instance;
            return instance;
        }

        // Return a unique id value
        ul_uid get_uid() {
            return ++current_id;
        }
};

#endif