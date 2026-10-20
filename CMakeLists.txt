#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.27)
project(MedLinac)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#

# Allow user to specify iAIDA include dir and library, or try to find them
set(IAIDA_INCLUDE_DIR "/usr/local/share/doc/iAIDA/src" CACHE PATH "Path to iAIDA include directory")
if(APPLE)
  set(IAIDA_LIBRARY "/usr/local/lib/libiAIDA.dylib" CACHE FILEPATH "Path to iAIDA library")
else()
  set(IAIDA_LIBRARY "/usr/local/lib/libiAIDA.so" CACHE FILEPATH "Path to iAIDA library")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${Geant4_INCLUDE_DIR}
  ${IAIDA_INCLUDE_DIR}
)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(MedLinac MedLinac.cc ${sources} ${headers})
target_link_libraries(MedLinac ${Geant4_LIBRARIES} ${IAIDA_LIBRARY})
target_compile_definitions(MedLinac PRIVATE G4ANALYSIS_USE)
#target_compile_definitions(MedLinac PRIVATE G4VIS_USE)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build MedLinac. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(MedLinac_SCRIPTS
    macro.mac
    vis.mac
  )

foreach(_script ${MedLinac_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
#add_custom_target(MedLinac DEPENDS ml2)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS MedLinac DESTINATION bin)

