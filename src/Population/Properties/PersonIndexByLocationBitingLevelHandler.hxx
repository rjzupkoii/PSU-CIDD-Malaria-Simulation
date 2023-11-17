/* 
 * PersonIndexByLocationBitingLevelHandler.hxx
 *
 * Define the interface for the person index by location and biting level handler.
 */
#ifndef PERSON_INDEX_BY_LOCATION_BITING_LEVEL_HANDLER_HXX
#define PERSON_INDEX_BY_LOCATION_BITING_LEVEL_HANDLER_HXX

#include "Core/PropertyMacro.h"
#include "IndexHandler.hxx"

class PersonIndexByLocationBitingLevelHandler : public IndexHandler {

 DISALLOW_COPY_AND_ASSIGN(PersonIndexByLocationBitingLevelHandler)

 public:
  PersonIndexByLocationBitingLevelHandler() = default;

  ~PersonIndexByLocationBitingLevelHandler() override = default;

};

#endif
