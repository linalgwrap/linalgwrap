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

if (BUILD_EXTERNAL_RAPIDCHECK)
	# Check whether rapidcheck is there
	if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/rapidcheck/CMakeLists.txt")
		message(FATAL_ERROR "Could not find rapidcheck at expected place. Try a manual git checkout.")
	endif()

	# Set option such that rapidcheck tests are built.
	set(RC_ENABLE_TESTS ON CACHE BOOL "Build RapidCheck tests")

	# Change compiler flags (CMAKE_CXX_FLAGS) to make fresh build config
	set(CMAKE_CXX_FLAGS_STORED_TMP ${CMAKE_CXX_FLAGS})
	set(CMAKE_CXX_FLAGS "")
	#enable_if_compiles(CMAKE_CXX_FLAGS "-Wno-gnu-zero-variadic-macro-arguments")

	# Add the rapidcheck subdirectory and configure its built.
	add_subdirectory(rapidcheck)

	# undo the changes to the compiler flags:
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_STORED_TMP})
	unset(CMAKE_CXX_FLAGS_STORED_TMP)
endif()

if (BUILD_EXTERNAL_KRIMS)
	# Check whether krims is there
	if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/krims/CMakeLists.txt")
		message(FATAL_ERROR "Could not find krims at expected place. Try a manual git checkout.")
	endif()

	add_subdirectory(krims)
endif()
