/**
 * @copyright
 * ====================================================================
 * Copyright (c) 2003-2004 CollabNet.  All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at http://subversion.tigris.org/license-1.html.
 * If newer versions of this license are posted there, you may use a
 * newer version instead, at your option.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://subversion.tigris.org/.
 * ====================================================================
 * @endcopyright
 *
 */
/**
 * Modified by Zoltan Molnar, Vanderbilt University, 2008
 */

#pragma once
#include "svn_client.h"
#include "apr.h"
#include <string>

/**
 * Class of utility methods. No Objects of this class are ever created
 */

class Util
{
public:
	static bool globalInit();

	static svn_error_t* preprocessPath(const char* &path, apr_pool_t* pool);

	static void throwNullPointerException(const char*);

	static void assembleErrorMessage(svn_error_t* err, int depth,
	                           apr_status_t parent_apr_err,
	                           std::string& buffer);

	static void handleSVNError(svn_error_t* p_err);
private:
	Util();
};
