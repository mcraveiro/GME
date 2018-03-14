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

#pragma once

#include <string>
#include <apr_pools.h>
struct svn_error_t;

/**
* Encapsulation for Subversion Path handling
*/
class Path  
{
private:
	// the path to be stored
	std::string m_path;
	apr_pool_t* m_pool;

	svn_error_t *m_error_occured;

	/**
	* initialize the class
	*
	* @param pi_path Path string
	*/
	void init(const char * pi_path, apr_pool_t* pool);

public:
	/**
	* Constructor that takes a string as parameter.
	* The string is converted to subversion internal
	* representation. The string is copied.
	*
	* @param pi_path Path string
	*/
	Path(const std::string & pi_path, apr_pool_t* pool);

	/**
	* Constructor
	*
	* @see Path::Path (const std::string &)
	* @param pi_path Path string
	*/
	Path(const char * pi_path, apr_pool_t* pool);

	/**
	* Copy constructor
	*
	* @param pi_path Path to be copied
	*/
	Path(const Path & pi_path);

	/**
	* Assignment operator
	*/
	Path& operator=(const Path&);

	/**
	* @return Path string
	*/
	const std::string &
		path() const;

	/**
	* @return Path string as c string
	*/
	const char * 
		c_str() const;

	svn_error_t * 
		error_occured() const;

	/**
	* Returns whether @a path is non-NULL and passes the @c
	* svn_path_check_valid() test.
	*
	* @since 1.4.0
	*/
	static bool isValid(const char *path);
};
