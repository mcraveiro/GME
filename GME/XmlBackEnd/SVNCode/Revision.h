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

#include "svn_opt.h"

class Revision
{
private:
	svn_opt_revision_t m_revision;

public:
	static const svn_opt_revision_kind START;
	static const svn_opt_revision_kind HEAD;

	Revision();
	Revision( bool p_head /*= false*/, bool p_one /*= false*/);
	Revision( const svn_opt_revision_kind p_kind);
	~Revision();

	const svn_opt_revision_t *revision() const;

};
