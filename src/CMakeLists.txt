# Enable for compiler warnings on core simulation files
set(BUILD_WARNINGS false CACHE BOOL "Enable for build warnings")

# Enable if building on the Windows Subsystem for Linux (WSL), via the command line use -DBUILD_WSL:BOOL=true
set(BUILD_WSL false CACHE BOOL "Enable for builds on WSL")

include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

# Define the core simulation files
file(GLOB SOURCE *.cpp 
        Core/*.cpp 
        Core/Config/*.cpp 
        Core/Config/Decoders/*.cpp
        Core/Index/*.cpp 
        Events/*.cpp 
        Events/Population/*.cpp 
        GIS/*.cpp
        Helpers/*.cpp 
        Malaria/*.cpp
        MDC/*.cpp 
        Parasites/*.cpp 
        Population/*.cpp
        Population/Properties/*.cpp
        Reporters/*.cpp 
        Spatial/*.cpp 
        Strategies/*.cpp
        Therapies/*.cpp       
)

# Define extra library files
file(GLOB LIBRARY
        ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.h
        ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4/easylogging++.cc
)

set(SRC_FILES ${SOURCE} ${LIBRARY})

add_library(MaSimCore STATIC ${SRC_FILES})
add_dependencies(MaSimCore date args)

if (BUILD_WSL)
target_link_libraries(MaSimCore PUBLIC
        yaml-cpp
        GSL::gsl GSL::gslcblas
        fmt::fmt
        )
else()
target_link_libraries(MaSimCore PUBLIC
        yaml-cpp
        GSL::gsl GSL::gslcblas
        fmt::fmt fmt::fmt-header-only
        )
endif()

# Enable warnings when building debug
# -Wunused-parameter from -Wextra is disabled since the YAML parser will emit the warning
# -Wunused-variable from -Wall is disabled since easylogging++ frequently emits it
if (BUILD_WARNINGS)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")
endif()

# Update the linker for Libpqxx
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq")
set(PQXX /usr/local/include/pqxx)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

add_executable(MaSim MaSim/main.cpp)
add_dependencies(MaSim MaSimCore)

target_link_libraries(MaSim PRIVATE MaSimCore ${PQXX_LIB} ${PQ_LIB})
target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/misc/input_dev.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)


file(GLOB DXG_SRC_FILES DxGGenerator/*.cpp DxGGenerator/*.h)

add_executable(DxGGenerator ${DXG_SRC_FILES})
add_dependencies(DxGGenerator  MaSimCore)
target_link_libraries(DxGGenerator PRIVATE MaSimCore ${PQXX_LIB} ${PQ_LIB})
target_compile_features(DxGGenerator PRIVATE cxx_range_for)

#install(TARGETS DxGGenerator DESTINATION ${PROJECT_SOURCE_DIR}/bin)
#install(FILES ${PROJECT_SOURCE_DIR}/misc/input_DxG.yml DESTINATION ${PROJECT_SOURCE_DIR}/bin)

# Note the flags that we ran with
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Library Type: " ${LIB_TYPE})
message(STATUS "Compiler flags:" ${CMAKE_CXX_COMPILE_FLAGS})
message(STATUS "Compiler cxx debug flags:" ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "Compiler cxx release flags:" ${CMAKE_CXX_FLAGS_RELEASE})
message(STATUS "Compiler cxx min size flags:" ${CMAKE_CXX_FLAGS_MINSIZEREL})
message(STATUS "Compiler cxx flags:" ${CMAKE_CXX_FLAGS})