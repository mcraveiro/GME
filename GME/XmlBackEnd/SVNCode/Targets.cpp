/**
 * @copyright
 * ====================================================================
 * Copyright (c) 2003 CollabNet.  All rights reserved.
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

#include "../StdAfx.h"

#if(USESVN)

#include "Targets.h"
#include "Pool.h"
#include "Util.h"
#include <apr_tables.h>
#include <apr_strings.h>
#include <svn_path.h>
#include <iostream> // remove this later
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Targets::~Targets()
{
	m_targets.clear();
}

Targets::Targets(const char *path, apr_pool_t* pool)
{
	m_pool = pool;
	m_targets.push_back(Path(path, pool));
	m_error_occured = NULL;
	m_doesNotContainsPath = false;
}

void Targets::add(const char *path)
{
	m_targets.push_back(Path(path, m_pool));
}

const apr_array_header_t *Targets::array (const Pool & pool)
{
	std::vector<Path>::const_iterator it;

	apr_pool_t *apr_pool = pool.pool();
	apr_array_header_t *apr_targets =
		apr_array_make (apr_pool,
		m_targets.size(),
		sizeof (const char *));

	for (it = m_targets.begin (); it != m_targets.end (); ++it)
	{
		const Path &path = *it;
		const char * target =
			apr_pstrdup (apr_pool, path.c_str());
		(*((const char **) apr_array_push (apr_targets))) = target;
	}

	return apr_targets;
}

svn_error_t *Targets::error_occured()
{
	return m_error_occured;
}

void Targets::setDoesNotContainsPath()
{
	m_doesNotContainsPath = true;
}

#endif
