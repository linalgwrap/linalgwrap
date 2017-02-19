## ---------------------------------------------------------------------
##
## Copyright (C) 2016-17 by the linalgwrap authors
##
## This file is part of linalgwrap.
##
## linalgwrap is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## linalgwrap is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with linalgwrap. If not, see <http://www.gnu.org/licenses/>.
##
## ---------------------------------------------------------------------

#
# set the source files.
#
set(LINALGWRAP_SOURCES
	version.cc
	io/Mathematica.cc
	Base/Interfaces/OperatorProperties.cc
	Base/Interfaces/Transposed.cc
	Base/Solvers/EigensolverBaseKeys.cc
	Base/Solvers/IterativeWrapperKeys.cc
	Base/Solvers/LinearSolverBaseKeys.cc
	Base/Solvers/SolverStateBase.cc
	Arpack/ArpackEigensolver.cc
	LinearSolver.cc
	EigensystemSolver.cc
	rescue.cc
)

# Dump the current version as an include file.
include_krims_cmake_module(WriteVersionHeader)
WRITE_VERSION_HEADER("${linalgwrap_BINARY_DIR}/src/linalgwrap/version_defs.hh")

# add the libraries for Debug and Release:
drb_add_library(linalgwrap
	DBGSUFFIX  ".g"
	FILES ${LINALGWRAP_SOURCES}
)

drb_set_target_properties(ALL linalgwrap
	PROPERTIES
	VERSION "${PROJECT_VERSION}"
)

drb_target_compile_definitions(ALL     linalgwrap
	PUBLIC ${LINALGWRAP_DEFINITIONS})
drb_target_compile_definitions(DEBUG   linalgwrap
	PUBLIC ${LINALGWRAP_DEFINITIONS_DEBUG})
drb_target_compile_definitions(RELEASE linalgwrap
	PUBLIC ${LINALGWRAP_DEFINITIONS_RELEASE})

drb_target_link_libraries(ALL     linalgwrap ${LINALGWRAP_DEPENDENCIES})
drb_target_link_libraries(DEBUG   linalgwrap ${LINALGWRAP_DEPENDENCIES_DEBUG})
drb_target_link_libraries(RELEASE linalgwrap ${LINALGWRAP_DEPENDENCIES_RELEASE})

# set the installation property for these targets:
install(TARGETS ${linalgwrap_TARGETS}
	EXPORT LinalgwrapTargets
	COMPONENT library
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

# install the includes:
install(DIRECTORY .
	DESTINATION "include/linalgwrap"
	COMPONENT devel
	FILES_MATCHING PATTERN "*.hh"
)
install(FILES "${linalgwrap_BINARY_DIR}/src/linalgwrap/version_defs.hh"
	DESTINATION "include/linalgwrap"
	COMPONENT devel
)

# Export the target specifications for linalgwrap.
export(EXPORT LinalgwrapTargets
	FILE "${linalgwrap_BINARY_DIR}/linalgwrapTargets.cmake"
	NAMESPACE Upstream::
)

# and install them appropriately:
install(EXPORT LinalgwrapTargets
	FILE "linalgwrapTargets.cmake"
	NAMESPACE Upstream::
	DESTINATION "${PackageModuleLocation}/linalgwrap"
)

