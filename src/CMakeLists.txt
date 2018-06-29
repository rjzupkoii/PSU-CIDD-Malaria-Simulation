file(GLOB SRC_FILES *.cpp *.h Spatial/*.cpp Spatial/*.h Reporters/*.cpp Reporters/*.h Strategies/*.cpp Strategies/*.h
    MDC/*.cpp MDC/*.h Helpers/*.cpp Helpers/*.h)

add_executable(MaSim ${SRC_FILES} ObjectPool.h)
#if (YAMLCPP_FOUND)
#  add_dependencies(MaSim yaml-cpp)
#else ()
#  add_dependencies(MaSim yaml-cpp-ext)
#endif ()
add_dependencies(MaSim date spdlog fmt)

target_link_libraries(MaSim
    ${YAMLCPP_LIBRARY_DIR}/libyaml-cpp.a
    ${FMT_LIBS_DIR}/libfmt.a
    ${GSL_LIBRARIES}
    )

target_compile_features(MaSim PRIVATE cxx_range_for)

install(TARGETS MaSim DESTINATION ${PROJECT_SOURCE_DIR}/bin)

