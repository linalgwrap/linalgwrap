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

# sets these things
#
# 	LINALGWRAP_DEPENDENCIES			everyone needs these libraries
# 	LINALGWRAP_DEPENDENCIES_DEBUG		debug mode needs these extras
# 	LINALGWRAP_DEPENDENCIES_RELEASE		release mode needs these extras
# 	LINALGWRAP_DEPENDENCIES_TEST		tests need these extra libraries
#
#       LINALGWRAP_DEFINITIONS			definitions for all compilation
#       LINALGWRAP_DEFINITIONS_DEBUG		definitions for debug mode
#       LINALGWRAP_DEFINITIONS_RELEASE		definitions for release mode
#       

####################
#-- Empty it all --#
####################
set(LINALGWRAP_DEPENDENCIES "")
set(LINALGWRAP_DEPENDENCIES_DEBUG "")
set(LINALGWRAP_DEPENDENCIES_RELEASE "")
set(LINALGWRAP_DEPENDENCIES_TEST "")
set(LINALGWRAP_DEFINITIONS "")
set(LINALGWRAP_DEFINITIONS_DEBUG "")
set(LINALGWRAP_DEFINITIONS_RELEASE "")

#############
#-- krims --#
#############
# Switch off building external krims by default.
# (Needs to be done here such that in the current scope by default
# this value is off, even if krims is build as a subproject to
# another project where this variable was set to ON.)
set(BUILD_EXTERNAL_KRIMS off)

# Find at least version 0.0.0
set(KRIMS_VERSION 0.0.0)
include(cmake/findKrims.cmake)

foreach (build ${DRB_BUILD_TYPES})
	set(LINALGWRAP_DEPENDENCIES_${build} ${LINALGWRAP_DEPENDENCIES_${build}} ${krims_${build}_TARGET})
endforeach()

############################
#-- rapidcheck and catch --#
############################
# Switch off building external rapidcheck by default.
# (see above for reasons)
set(BUILD_EXTERNAL_RAPIDCHECK off)

if (LINALGWRAP_ENABLE_TESTS)
	# We need to setup rapidcheck and catch for the tests:
	include(cmake/findRapidcheck.cmake)
	set(LINALGWRAP_DEPENDENCIES_TEST ${LINALGWRAP_DEPENDENCIES_TEST} ${rapidcheck_TARGET})

	include(cmake/findCatch.cmake)
	set(LINALGWRAP_DEPENDENCIES_TEST ${LINALGWRAP_DEPENDENCIES_TEST} ${catch_TARGET})
endif()

#################
#-- armadillo --#
#################
include(FindArmadillo)
find_package(Armadillo REQUIRED)

# We need at least 4.000
if(ARMADILLO_VERSION_STRING VERSION_LESS "4.000")
	message(FATAL_ERROR "Armadillo version is too old. Expect at least version ${ARMADILLO_MIN_VERSION}.")
endif()

# add to general dependencies and include string
set(LINALGWRAP_DEPENDENCIES ${LINALGWRAP_DEPENDENCIES} ${ARMADILLO_LIBRARIES})
include_directories(${ARMADILLO_INCLUDE_DIRS})

# enable armadillo-dependant code:
LIST(APPEND LINALGWRAP_DEFINITIONS "LINALGWRAP_HAVE_ARMADILLO")
