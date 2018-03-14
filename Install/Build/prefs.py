#!/usr/bin/python
#
# Copyright (c) 2006-2008 ISIS, Vanderbilt University
#
# Author: Peter Volgyesi (peter.volgyesi@vanderbilt.edu)
#
"""GME Build System - preferences module"""

import time

#
# Preferences & default values (used by other modules)
#
prefs = {
	"clean" : False,
	"verbose" : False,
	"version_major" : (time.localtime()[0] - 2000),
	"version_minor" : time.localtime()[1],
	"version_patch" : time.localtime()[2],
	"version_build" : 0,
	"arch" : "x86",
	"toolset": "10",
	}