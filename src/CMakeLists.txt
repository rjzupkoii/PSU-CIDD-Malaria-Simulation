file(GLOB SRC_FILES *.cpp *.h Spatial/*.cpp Spatial/*.h Reporters/*.cpp Reporters/*.h Strategies/*.cpp Strategies/*.h
    MDC/*.cpp MDC/*.h Helpers/*.cpp Helpers/*.h)

add_executable(MaSim ${SRC_FILES} ObjectPool.h)
add_dependencies(MaSim date spdlog)

target_link_libraries(MaSim
    yaml-cpp
    fmt::fmt
     GSL::gsl GSL::gslcblas
    )

target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)

