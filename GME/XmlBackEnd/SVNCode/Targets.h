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

struct apr_array_header_t;
struct svn_error_t;
class Pool;
#include "Path.h"
#include <vector>
#include <apr_pools.h>

class Targets
{
private:
	std::vector<Path>     m_targets;
	svn_error_t *         m_error_occured;
	bool                  m_doesNotContainsPath;
	apr_pool_t* m_pool;

public:
	Targets(const char *path, apr_pool_t* pool);
	void add(const char *path);
	~Targets();

	const apr_array_header_t *         array(const Pool & pool);
	svn_error_t *                      error_occured();
	void                               setDoesNotContainsPath();
};
