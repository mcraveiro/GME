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

#pragma once

#include "Revision.h"
#include "Targets.h"
#include "svn_client.h"
#include "Prompter.h"
#include "ClientUtil.h"
#include "Pool.h"

// class mimicing subversion/bindings/java/javahl/native/svnclient.cpp
class CommitMessage;

class Client
{
public:
	//Client(void);
	Client( const std::string& p_userName, const std::string& p_passWord);
	virtual ~Client(void);

protected:
	// not so public: signaled by the sub_ prefix
	long sub_checkout(const char *moduleName, const char *destPath, 
		Revision &revision, Revision &pegRevision, 
		bool recurse, bool ignoreExternals);

	typedef std::vector< long> UPDATE_RES;

	bool sub_update(Targets &targets, Revision &revision, bool recurse,
		bool ignoreExternals, UPDATE_RES& res);

	long sub_commit(Targets &targets, const char *message, bool recurse,
		bool noUnlock);
	long sub_commit3(Targets &targets, const char *message, bool recurse,
		bool noUnlock);

	bool sub_cleanup( const char *path);
	bool sub_resolved(const char *path, bool recurse);

	bool sub_propertySet(const char *path, const char *name, 
		const char *value, bool recurse, bool force);

	bool sub_lock(Targets &targets, const char *comment, 
		bool force);

	bool sub_unlock(Targets &targets);

	bool sub_info2( const char * p_path, Revision &revision, Revision &pegRevision, bool p_recurse, ClientUtil::InfoHelp::InfoVec& p_infoVect, bool p_suppressIllegalUrlErrorMsg);
	bool info2Qck ( const char * p_path, bool p_recursive, ClientUtil::InfoHelp::InfoVec& p_infoVect, bool p_suppressErrorMsg = false);

	int sub_add( const char *p_pathOrUrl, bool rec);
	

	long sub_mkdir( Targets& targets, const char * p_msg);
	long sub_mkdir2( Targets& targets, const char * p_msg);

	std::auto_ptr<Pool> m_ctxPool;
	svn_client_ctx_t* m_ctx;
	std::string m_commitMessage;
	svn_client_ctx_t * createContext(const char *p_message, apr_pool_t *pool);
	svn_client_ctx_t * getContext(const char *p_message, apr_pool_t *pool);

	//void * getCommitMessageBaton(const char *message);


	ClientUtil::NotifyHelp *m_notify2;
	void notification2( ClientUtil::NotifyHelp *notify2);

    Prompter *m_prompter;
	void setPrompt(Prompter *prompter);

	ClientUtil::CommitHelp::MsgBuilder * m_commitMessageBuilder;
	void setCommitMessageHandler( ClientUtil::CommitHelp::MsgBuilder * p_commitMessageBuilder);

	std::string m_userName;
	std::string m_passWord;
	std::string m_configDir;

	void cancelOperation();


	ClientUtil::StatusInfo sub_single_status(const char *path, bool onServer);
	ClientUtil::StatusExtInfoVec sub_extended_status( const char * p_path, bool p_onServer);

public:
	bool m_cancelOperation; // client can signal her patience is not endless
	                        // accessed from batons as well
};

