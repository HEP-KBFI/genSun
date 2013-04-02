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
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
# ---
set(source_files
	SunDetectorConstruction.cc
	DMPhysicsList.cc
	DMPrimaryGeneratorAction.cc
)
set(include_files
	SunDetectorConstruction.hh
	DMPhysicsList.hh
	DMPrimaryGeneratorAction.hh
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
target_link_libraries(solnugeant ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
# ---
set(SCRIPTS
	default.mac
	vis.mac
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

