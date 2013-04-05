#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(SOLNUGEANT)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
# ---
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
	find_package(Geant4 REQUIRED ui_all vis_all)
else()
	find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
# ---
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Check for PYTHIA8
# ---
if(DEFINED ENV{PYTHIA8})
	message("$PYTHIA8 value: " $ENV{PYTHIA8})
else()
	message(FATAL_ERROR
		"ERROR: Variable $PYTHIA8 not defined!" "\n"
		"Make sure it points to PYTHIA install directory."
	)
	exit()
endif()
include_directories($ENV{PYTHIA8}/include)
FIND_LIBRARY(PYTHIA8_LIBRARY pythia8 $ENV{PYTHIA8}/lib/archive)
FIND_LIBRARY(HAPDFDUMMY_LIBRARY lhapdfdummy  $ENV{PYTHIA8}/lib/archive)
message("pythia lib" ${PYTHIA8_LIBRARY})
#TARGET_LINK_LIBRARIES(pythia8 $ENV{PYTHIA8}/lib)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
# ---
set(source_files
	SunDetectorConstruction.cc
	DMPhysicsList.cc
	DMPrimaryGeneratorAction.cc
	NeutrinoStackingAction.cc
	NeutrinoHistogram.cc
	DMPythiaPGA.cc
	SunSteppingAction.cc
)
set(include_files
	SunDetectorConstruction.hh
	DMPhysicsList.hh
	DMPrimaryGeneratorAction.hh
	NeutrinoStackingAction.hh
	NeutrinoHistogram.hh
	DMPythiaPGA.hh
	SunSteppingAction.hh
)

message(" > Sources...")
set(sources)
foreach(source ${source_files})
	set(source_full "${PROJECT_SOURCE_DIR}/src/${source}")
	list(APPEND sources ${source_full})
	message(" > - ${source_full}")
endforeach(source)

message(" > Headers...")
set(headers)
foreach(include ${include_files})
	set(header_full "${PROJECT_SOURCE_DIR}/include/${include}")
	list(APPEND sources ${header_full})
	message(" > - ${header_full}")
endforeach(include)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
# ---
add_executable(solnugeant main.cc ${sources} ${headers})
target_link_libraries(solnugeant
	${Geant4_LIBRARIES}
	${PYTHIA8_LIBRARY} ${HAPDFDUMMY_LIBRARY}
)

add_executable(test_dmpythia test/dmpythia.cc ${sources} ${headers})
target_link_libraries(test_dmpythia
	${Geant4_LIBRARIES}
	${PYTHIA8_LIBRARY} ${HAPDFDUMMY_LIBRARY}
)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
# ---
set(SCRIPTS
	default.mac
	vis.mac
	pythia.card
)

message(" > Copying scripts...")
foreach(_script ${SCRIPTS})
	set(script_from ${PROJECT_SOURCE_DIR}/scripts/${_script})
	configure_file(
		${script_from}
		${PROJECT_BINARY_DIR}/${_script}
		COPYONLY
	)
	message(" > - ${script_from}")
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
# ---
add_custom_target(SOLNUGEANT DEPENDS solnugeant)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
# ---
install(TARGETS solnugeant DESTINATION bin)


