#!/bin/sh
## ---------------------------------------------------------------------
##
## Copyright (C) 2016-17 by the lazyten authors
##
## This file is part of lazyten.
##
## lazyten is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## lazyten is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with lazyten. If not, see <http://www.gnu.org/licenses/>.
##
## ---------------------------------------------------------------------

## DO NOT EDIT
## This file is automatically generated from a file in the repository "krims".
## Edit the original and call the script "update_from_sister_repos.sh" instead.

#
# Settings
#
# The git repo to checkout
FROM="https://github.com/emil-e/rapidcheck.git"

# Folder to check it out to
WHAT="rapidcheck"

# Interval: How often to update (in seconds)
INTERVAL="3600"

# File to use in order to test a successful checkout
CHECKFILE="CMakeLists.txt"

# Branch to checkout (empty for default)
BRANCH="9eb1e620cf84aa6bec638ef7bb1ed6e4989f43a2"

if [ ! -f "$PWD/get.lib.sh" ]; then
	echo "PWD needs to be the location of the get.lib.sh file."
	exit 1
fi

. "$PWD/get.lib.sh" || exit 1
update_repo "$FROM" "$WHAT" "$CHECKFILE" "$INTERVAL" "$BRANCH"
exit $?
