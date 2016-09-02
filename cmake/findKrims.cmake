## ---------------------------------------------------------------------
##
## Copyright (C) 2016 by the linalgwrap authors
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

# Finds and sets up krims under the target names stored in
#      krims_DEBUG_TARGET     (Debug version)
#      krims_RELEASE_TARGET   (Release version)
# such that just linking against it as a dependency does everything
# automatically.
#
# In case the krims library is not found and AUTOCHECKOUT_MISSING_LIBS is set to
# on, krims is automatically checked out and built.
# Otherwise a fatal error is produced.
#

#
# Options and properties required
#
option(AUTOCHECKOUT_MISSING_REPOS "Automatically checkout missing repositories" OFF)

#
# -------
#

if (TARGET "${krims_DEBUG_TARGET}"  OR TARGET "${krims_RELEASE_TARGET}")
	message(STATUS "Found target krims, assume krims already configured for build.")
	return()
endif()

# Try to find krims somewhere
find_package(krims ${KRIMS_VERSION} QUIET CONFIG)
string(TOUPPER "${PROJECT_NAME}" PROJECT_UPPER)
if ("${krims_DIR}" STREQUAL "krims_DIR-NOTFOUND")
	if (AUTOCHECKOUT_MISSING_REPOS)
		execute_process(
			COMMAND "sh" "get_krims.sh"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/external"
			RESULT_VARIABLE RES
		)
		if (NOT RES EQUAL 0)
			message(FATAL_ERROR "Getting krims from git failed with error: ${RES}")
		endif()

		# TODO check version of krims!

		set(BUILD_EXTERNAL_KRIMS on)
		return()
	endif()

	message(FATAL_ERROR "Could not find krims library.
Either provide the installation prefix of the krims library in the environment \
variable krims_DIR or enable autocheckout via -DAUTOCHECKOUT_MISSING_REPOS=ON.")
endif()

message(WARNING "This part of findKrims has never been tested.")

# Setup library targets
set(krims_DEBUG_TARGET   "Upstream::krims.g"
	CACHE INTERNAL "Target name of debug version of krims")
set(krims_RELEASE_TARGET "Upstream::krims"
	CACHE INTERNAL "Target name of release version of krims")

# Check that all required targets are available.
foreach(build ${DRB_BUILD_TYPES})
	if(NOT TARGET "${krims_${build}_TARGET}")
		message(FATAL_ERROR "We could not find a ${build} version of krims at this location. \
		Either disable building a ${build} version of ${CMAKE_PROJECT_NAME} or else \
		rebuild krims with a ${build} version as well.")
	endif()
endforeach()

#TODO check that we don't need extra stuff like in findLinalgwrap in gscf

message(STATUS "Found krims config at ${krims_CONFIG}")
