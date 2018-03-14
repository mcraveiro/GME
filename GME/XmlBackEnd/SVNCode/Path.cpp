/**
 * @copyright
 * ====================================================================
 * Copyright (c) 2003-2006 CollabNet.  All rights reserved.
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

#include "Path.h"
#include "svn_path.h"
#include "Util.h"
#include "Pool.h"

/**
* Constructor
*
* @see Path::Path (const std::string &)
* @param path Path string
*/
Path::Path (const char * pi_path, apr_pool_t* pool)
{
	init(pi_path, pool);
}

/**
* Constructor that takes a string as parameter.
* The string is converted to subversion internal
* representation. The string is copied.
*
* @param path Path string
*/
Path::Path (const std::string & pi_path, apr_pool_t* pool)
{
	init(pi_path.c_str(), pool);
}

/**
* Copy constructor
*
* @param path Path to be copied
*/
Path::Path (const Path & pi_path)
{
	init(pi_path.c_str(), pi_path.m_pool);
}

/**
* initialize the class
*
* @param path Path string
*/
void
Path::init(const char * pi_path, apr_pool_t* pool)
{
	m_pool = pool;
	if(*pi_path == 0)
	{
		m_error_occured = NULL;
		m_path = "";
	}
	else
	{
		// FIXME: hits the disk a lot
		//21	XmlBackEnd.dll	Util::preprocessPath + 0xf0, c:\users\ksmyth\git\gmesrc\gme\xmlbackend\svncode\util.cpp(317)	0x6f9f4a0	C:\Users\ksmyth\git\GMESRC\GME\Release\XmlBackEnd.dll
		//22	XmlBackEnd.dll	Path::Path + 0x6d, c:\users\ksmyth\git\gmesrc\gme\xmlbackend\svncode\path.cpp(39)	0x6f9b82d	C:\Users\ksmyth\git\GMESRC\GME\Release\XmlBackEnd.dll
		//23	XmlBackEnd.dll	Client::sub_propertySet + 0xb5, c:\users\ksmyth\git\gmesrc\gme\xmlbackend\svncode\client.cpp(357)	0x6f97945	C:\Users\ksmyth\git\GMESRC\GME\Release\XmlBackEnd.dll
		//24	XmlBackEnd.dll	HiClient::lockableProp + 0xd8, c:\users\ksmyth\git\gmesrc\gme\xmlbackend\svncode\hiclient.cpp(320)	0x6f98cc8	C:\Users\ksmyth\git\GMESRC\GME\Release\XmlBackEnd.dll

		m_error_occured = Util::preprocessPath(pi_path, m_pool);

		m_path = pi_path;
	}
}

/**
* @return Path string
*/
const std::string &
Path::path () const
{
	return m_path;
}

/**
* @return Path string as c string
*/
const char *
Path::c_str() const
{
	return m_path.c_str ();
}

/**
* Assignment operator
*/
Path&
Path::operator=(const Path & pi_path)
{
	init(pi_path.c_str(), pi_path.m_pool);
	return *this;
}

svn_error_t *Path::error_occured() const
{
	return m_error_occured;
}

bool Path::isValid(const char *p)
{
	if (p == NULL)
	{
		return false;
	}

	Pool reqPool;
	svn_error_t *err = svn_path_check_valid(p, reqPool.pool());
	if (err == SVN_NO_ERROR)
	{
		return true;
	}
	else
	{
		svn_error_clear(err);
		return false;
	}
}

#endif