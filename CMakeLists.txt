### Define project details ###
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(CoherenceFit)

### Add ROOT stuff ###
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO MathCore RooFit RooFitCore RooStats TMVA TMVAGui)
include(${ROOT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


### Add my libraries ###
include_directories(FitLib/FitLib)
file(GLOB FITLIB_SOURCES "FitLib/src/*.cpp")
add_library(FitLib STATIC
    ${FITLIB_SOURCES}
)


### Add main fitting code ###
SET (COMB_MACS
    cptags
    kspipi
)

foreach( mac ${COMB_MACS} )
    add_executable(${mac} ${CMAKE_CURRENT_SOURCE_DIR}/main/${mac}.cpp)
    target_link_libraries(${mac} FitLib -lMinuit -lMinuit2 ${ROOT_LIBRARIES})
    INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/${mac} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
endforeach()
