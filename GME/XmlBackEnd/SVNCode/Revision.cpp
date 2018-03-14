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

#include "Revision.h"

const svn_opt_revision_kind Revision::START = svn_opt_revision_unspecified;
const svn_opt_revision_kind Revision::HEAD  = svn_opt_revision_head;

Revision::Revision()
{
	m_revision.value.number = 0;
	m_revision.kind = svn_opt_revision_head;
}

Revision::Revision( bool p_head /* = false */, bool p_one /* = false */)
{
	m_revision.kind = svn_opt_revision_unspecified;
	m_revision.value.number = 0;

	if( p_head)
		m_revision.kind = svn_opt_revision_head;
	else if( p_one)
	{
		m_revision.kind = svn_opt_revision_number;
		m_revision.value.number = 1;
	}
}

Revision::Revision (const svn_opt_revision_kind p_kind)
{
	m_revision.kind = p_kind;
	m_revision.value.number = 0;
}

Revision::~Revision()
{
}

const svn_opt_revision_t *Revision::revision () const
{
	return &m_revision;
}

#endif
