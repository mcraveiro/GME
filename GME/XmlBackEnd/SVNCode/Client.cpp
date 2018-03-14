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

#include ".\client.h"
#include <crtdbg.h> // for ASSERT
//#include <iostream> // remove this later
#include "Pool.h"
#include "Util.h"
#include "svn_client.h"
#include "svn_config.h"

extern std::string g_userName;
extern std::string g_passWord;

Client::Client( const std::string& p_userName, const std::string& p_passWord)
	: m_notify2( 0)
	, m_prompter( 0)
	, m_commitMessageBuilder( 0)
	, m_userName( p_userName)
	, m_passWord( p_passWord)
{
	Util::globalInit();
	setPrompt( Prompter::makePrompter( new GPromptImpl()));
	// we set this:
	notification2( new ClientUtil::NotifyHelp());
	m_ctx = NULL;
	m_ctxPool = std::auto_ptr<Pool>(new Pool());
}

Client::~Client(void)
{
	if( m_prompter) delete m_prompter;
	m_prompter = 0;

	// should we delete this also?
	if( m_commitMessageBuilder) delete m_commitMessageBuilder;
	m_commitMessageBuilder = 0;

	notification2( 0);
}

void Client::notification2( ClientUtil::NotifyHelp *notify2)
{
	if( m_notify2) delete m_notify2;
	m_notify2 = notify2;
} 

void Client::setCommitMessageHandler( ClientUtil::CommitHelp::MsgBuilder* p_commitMessageBuilder)
{
    if( m_commitMessageBuilder) delete m_commitMessageBuilder;
    m_commitMessageBuilder = p_commitMessageBuilder;
}


void Client::setPrompt( Prompter * p_prompter)
{
    if( m_prompter) delete m_prompter;
    m_prompter = p_prompter;
}


// not so public interface:
long Client::sub_checkout(const char *moduleName, const char *destPath, 
						Revision &revision, Revision &pegRevision, 
						bool recurse, bool ignoreExternals)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();

	if(moduleName == NULL)
	{
		Util::throwNullPointerException("moduleName");
		return -1;
	}
	if(destPath == NULL)
	{
		Util::throwNullPointerException("destPath");
		return -1;
	}

	Path url(moduleName, reqPool.pool());
	Path path(destPath, reqPool.pool());
	svn_error_t *Err = url.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	Err = path.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	svn_revnum_t retval;

	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	if(ctx == NULL)
	{
		return -1;
	}

	Err = svn_client_checkout2 (&retval, url.c_str(),
		path.c_str (),
		pegRevision.revision (),
		revision.revision (),
		recurse, 
		ignoreExternals,
		ctx,
		apr_pool);

	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	return retval;

}

long Client::sub_commit(Targets &targets, const char *message, bool recurse,
					  bool noUnlock)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();
	svn_client_commit_info_t *commit_info = NULL;
	const apr_array_header_t *targets2 = targets.array(reqPool);
	svn_error_t *Err = targets.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	svn_client_ctx_t *ctx = getContext(message, apr_pool);
	if(ctx == NULL)
	{
		return -1;
	}
	Err = svn_client_commit2 (&commit_info,
		targets2,
		recurse, noUnlock, ctx, apr_pool);
	if(Err != NULL)
		Util::handleSVNError(Err);

	if(commit_info && SVN_IS_VALID_REVNUM (commit_info->revision))
		return commit_info->revision;

	return -1;
}

long Client::sub_commit3(Targets &targets, const char *message, bool recurse,
					  bool noUnlock)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();
	svn_commit_info_t *commit3_info = NULL;
	const apr_array_header_t *targets2 = targets.array(reqPool);
	svn_error_t *Err = targets.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	svn_client_ctx_t *ctx = getContext(message, apr_pool);
	if(ctx == NULL)
	{
		return -1;
	}
	Err = svn_client_commit3(&commit3_info,
		targets2,
		recurse, noUnlock, ctx, apr_pool);
	if(Err != NULL)
		Util::handleSVNError(Err);

#if(0)
	typedef struct svn_commit_info_t
	{
		svn_revnum_t revision;/** just-committed revision. */
		const char *date;/** server-side date of the commit. */
		const char *author;/** author of the commit. */
		const char *post_commit_err;/** error message from post-commit hook, or NULL. */
	} svn_commit_info_t;
#endif

	if(commit3_info && SVN_IS_VALID_REVNUM (commit3_info->revision))
		return commit3_info->revision;

	return -1;
}


bool Client::sub_update(Targets &targets, Revision &revision, bool recurse,
					bool ignoreExternals, std::vector< long>& res)
{
	Pool reqPoolParent;
	// This function leaks if we don't use two pools
	Pool reqPool(reqPoolParent);
	apr_pool_t * apr_pool = reqPool.pool();

	svn_client_ctx_t *ctx = getContext(NULL, apr_pool);
	apr_array_header_t *retval;
	if(ctx == NULL)
	{
		return false;
	}
	const apr_array_header_t *array = targets.array(reqPool);
	svn_error_t *Err = targets.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	try 
	{
		Err = svn_client_update2 (&retval, array,
			revision.revision (),
			recurse,
			ignoreExternals,
			ctx,
			apr_pool);
	}
	catch(...)
	{
		ASSERT(0);
		return false;
	}

	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	res.resize( retval->nelts);
	for( int i = 0; i < retval->nelts; ++i)
		res[i] = APR_ARRAY_IDX (retval, i, svn_revnum_t);

	return true;
}

bool Client::sub_cleanup(const char *path)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();
	if(path == NULL)
	{
		Util::throwNullPointerException("path");
		return false;
	}
	Path intPath(path, reqPool.pool());
	svn_error_t *Err = intPath.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	if(ctx == NULL)
	{
		return false;
	}
	Err = svn_client_cleanup (intPath.c_str (), ctx, apr_pool);

	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	return true;
}

bool Client::sub_resolved(const char *path, bool recurse)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();
	if(path == NULL)
	{
		Util::throwNullPointerException("path");
		return false;
	}
	Path intPath(path, reqPool.pool());
	svn_error_t *Err = intPath.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}
	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	if(ctx == NULL)
	{
		return false;
	}
	Err = svn_client_resolved (intPath.c_str (),
		recurse,
		ctx,
		apr_pool);

	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	return true;
}


bool Client::sub_propertySet(const char *path, const char *name, 
						const char *value, bool recurse, bool force)
{
	Pool reqPool;
	if(path == NULL)
	{
		Util::throwNullPointerException("path");
		return false;
	}
	if(name == NULL)
	{
		Util::throwNullPointerException("name");
		return false;
	}
	if(value == NULL)
	{
		Util::throwNullPointerException("value");
		return false;
	}
	svn_string_t *val = svn_string_create(value, reqPool.pool());


	//propertySet(path, name, val->data, recurse, force);                     // modified from val to val->data
    //Path intPath(path, reqPool.pool());
    svn_error_t *Err = NULL; // intPath.error_occured();
    if(Err != NULL)
    {
        Util::handleSVNError(Err);
        return false;
    }

    svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
    if(ctx == NULL)
        return false;
    Err = svn_client_propset2 (name, val, 
                                path,
                                recurse, 
                                force,
                                ctx,
								reqPool.pool());
    if(Err!= NULL)
	{
        Util::handleSVNError(Err); // previously just handled the error and fall through to return true
		return false;
	}

	return true;
}

bool Client::sub_lock(Targets &targets, const char *comment, 
		bool force)
{
	Pool reqPool;
	const apr_array_header_t *targetsApr = targets.array(reqPool);
	svn_error_t *Err = targets.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}
	apr_pool_t * apr_pool = reqPool.pool();
	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	Err = svn_client_lock(targetsApr, comment, force, ctx, apr_pool);

	if (Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	return true;
}

//svn_error_t *
//svn_client_unlock(const apr_array_header_t *targets,
//                  svn_boolean_t break_lock,
//                  svn_client_ctx_t *ctx,
//                  apr_pool_t *pool)

bool Client::sub_unlock(Targets &targets)
{
	Pool reqPool;
	const apr_array_header_t *targetsApr = targets.array(reqPool);
	svn_error_t *Err = targets.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}
	apr_pool_t * apr_pool = reqPool.pool();
	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	Err = svn_client_unlock(targetsApr, true, ctx, apr_pool);

	if (Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	return true;
}


ClientUtil::StatusInfo Client::sub_single_status(const char *path, bool onServer)
{
	ClientUtil::StatusInfo null_pair;
	ClientUtil::StatusHelp::StatusBaton statusBaton;
	Pool reqPool;
	svn_revnum_t youngest = SVN_INVALID_REVNUM;
	svn_opt_revision_t rev;

	if(path == NULL)
	{
		Util::throwNullPointerException("path");
		return null_pair;
	}

	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	if(ctx == NULL)
	{
		return null_pair;
	}


	rev.kind = svn_opt_revision_unspecified;
	statusBaton.m_pool = reqPool.pool();
	Path intPath(path, reqPool.pool());
	svn_error_t *Err = intPath.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return null_pair;
	}

	Err = svn_client_status2 (&youngest, intPath.c_str(), &rev, 
		ClientUtil::StatusHelp::statusReceiver, // callback of type svn_wc_status_func2_t
		&statusBaton,   // callback's parameter
		FALSE,          // DESCEND
		TRUE,           // get_All
		onServer ? TRUE : FALSE,     //update
		FALSE,          // no_ignore,
		FALSE,          // ignore externals
		ctx,
		reqPool.pool());

	if(Err == NULL)
	{
		int size = (int) statusBaton.m_statusVect.size();
		if (size == 0)
			return null_pair;

		// when svn_client_status is used with a directory, the status of the 
		// directory itself and the status of all its direct children are 
		// returned
		// we just want the status of the directory (ie the status of the 
		// element with the shortest path)
		int j = 0;
		for (int i = 0; i < size; i++)
		{
			if (strlen(statusBaton.m_statusVect[i].m_path) < 
				strlen(statusBaton.m_statusVect[j].m_path))
				j = i;
		}

		null_pair.first = statusBaton.m_statusVect[j].m_path;
		null_pair.second = (statusBaton.m_statusVect[j].m_status)? 1:0;

		//typedef struct svn_wc_status2_t
		//{
		//	svn_wc_entry_t *entry;
		//	enum svn_wc_status_kind text_status;
		//	enum svn_wc_status_kind prop_status;
		//	svn_boolean_t locked;
		//	svn_boolean_t copied;
		//	svn_boolean_t switched;
		//	enum svn_wc_status_kind repos_text_status;
		//	enum svn_wc_status_kind repos_prop_status;
		//	svn_lock_t *repos_lock;
		//	const char *url;
		//	svn_revnum_t ood_last_cmt_rev;
		//	apr_time_t ood_last_cmt_date;
		//	svn_node_kind_t ood_kind;
		//	const char *ood_last_cmt_author;
		//} svn_wc_status2_t;

		return null_pair;
	}
	else
	{
		Util::handleSVNError(Err);
		return null_pair;
	}
}

// locked status not reported correctly
// use info instead
ClientUtil::StatusExtInfoVec Client::sub_extended_status( const char *p_path, bool p_onServer)
{
	ClientUtil::StatusExtInfoVec res;
	ClientUtil::StatusHelp::StatusBaton statusBaton;
	Pool reqPool;
	svn_revnum_t youngest = SVN_INVALID_REVNUM;
	svn_opt_revision_t rev;

	if(p_path == NULL)
	{
		Util::throwNullPointerException("p_path");
		return res;
	}

	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	if(ctx == NULL)
	{
		return res;
	}


	rev.kind = svn_opt_revision_unspecified;
	statusBaton.m_pool = reqPool.pool();
	Path intPath(p_path, reqPool.pool());
	svn_error_t *Err = intPath.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return res;
	}

	Err = svn_client_status2 (&youngest, intPath.c_str(), &rev, 
		ClientUtil::StatusHelp::statusReceiver, // callback of type svn_wc_status_func2_t
		&statusBaton,   // callback's parameter
		FALSE,          // DESCEND
		TRUE,           // get_All
		p_onServer ? TRUE : FALSE,     //update
		FALSE,          // no_ignore,
		FALSE,          // ignore externals
		ctx,
		reqPool.pool());

	if(Err == NULL)
	{
		int size = (int) statusBaton.m_statusVect.size();
		if (size == 0)
			return res;

		// when svn_client_status is used with a directory, the status of the 
		// directory itself and the status of all its direct children are 
		// returned
		// we just want the status of the directory (ie the status of the 
		// element with the shortest path)
		int j = 0;
		for (int i = 0; i < size; i++)
		{
			//if( statusBaton.m_statusVect[i].m_status->locked)
			//	std::cout << "Stat " << "Locked" << std::endl;
			//else
			//	std::cout << "Stat " << "NotLocked" << std::endl;
			ClientUtil::StatusExtInfo r = statusBaton.m_statusVect[i];
			res.push_back( r);
		}

		//typedef struct svn_wc_status2_t
		//{
		//	svn_wc_entry_t *entry;
		//	enum svn_wc_status_kind text_status;
		//	enum svn_wc_status_kind prop_status;
		//	svn_boolean_t locked;
		//	svn_boolean_t copied;
		//	svn_boolean_t switched;
		//	enum svn_wc_status_kind repos_text_status;
		//	enum svn_wc_status_kind repos_prop_status;
		//	svn_lock_t *repos_lock;
		//	const char *url;
		//	svn_revnum_t ood_last_cmt_rev;
		//	apr_time_t ood_last_cmt_date;
		//	svn_node_kind_t ood_kind;
		//	const char *ood_last_cmt_author;
		//} svn_wc_status2_t;

		return res;
	}
	else
	{
		Util::handleSVNError(Err);
		return res;
	}
}


svn_client_ctx_t * Client::getContext(const char *p_strMessage, apr_pool_t *pool)
{
	if (m_ctx == NULL)
	{
		m_ctx = createContext(NULL, m_ctxPool->pool());
	}
	m_commitMessage = p_strMessage ? p_strMessage : "Empty";
	((ClientUtil::CommitHelp::LogMsgBaton*)m_ctx->log_msg_baton2)->m_message = m_commitMessage.c_str();
	return m_ctx;
}

svn_client_ctx_t * Client::createContext(const char *p_strMessage, apr_pool_t *pool)
{
	svn_auth_baton_t *ab;
	svn_client_ctx_t *ctx;
	svn_error_t *err = NULL;
	if (( err = svn_client_create_context(&ctx, pool)))
	{
		Util::handleSVNError(err);
		return NULL;
	}

	apr_array_header_t *providers
		= apr_array_make (pool, 10, sizeof (svn_auth_provider_object_t *));

	/* The main disk-caching auth providers, for both
	'username/password' creds and 'username' creds.  */
	svn_auth_provider_object_t *provider; // provider->vtable->save_credentials is a function pointer to saving to the cache
#ifdef WIN32
	svn_client_get_windows_simple_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
#endif
	svn_client_get_simple_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	svn_client_get_username_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

	/* The server-cert, client-cert, and client-cert-password providers. */
	svn_client_get_ssl_server_trust_file_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	svn_client_get_ssl_client_cert_file_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	svn_client_get_ssl_client_cert_pw_file_provider (&provider, pool);
	APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

	if(m_prompter != NULL)
	{
		/* Two basic prompt providers: username/password, and just username.*/
		provider = m_prompter->getProviderSimple(pool);
		APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

		provider = m_prompter->getProviderUsername(pool);
		APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

		/* Three ssl prompt providers, for server-certs, client-certs,
		and client-cert-passphrases.  */
		provider = m_prompter->getProviderServerSSLTrust(pool);
		APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

		provider = m_prompter->getProviderClientSSL(pool);
		APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;

		provider = m_prompter->getProviderClientSSLPassword(pool);
		APR_ARRAY_PUSH (providers, svn_auth_provider_object_t *) = provider;
	}



	/* Build an authentication baton to give to libsvn_client. */
	svn_auth_open (&ab, providers, pool);

	/* Place any default --username or --password credentials into the
	auth_baton's run-time parameter hash.  ### Same with --no-auth-cache? */
	if (!m_userName.empty())
		svn_auth_set_parameter(ab, SVN_AUTH_PARAM_DEFAULT_USERNAME, m_userName.c_str());
	else if( !g_userName.empty()) // g_userName is NOT empty => g_userName != m_userName
		svn_auth_set_parameter(ab, SVN_AUTH_PARAM_DEFAULT_USERNAME, g_userName.c_str()); // this might have been filled by prompt()
	
	if (!m_passWord.empty())
		svn_auth_set_parameter(ab, SVN_AUTH_PARAM_DEFAULT_PASSWORD, m_passWord.c_str());
	else if( !g_passWord.empty()) // g_passWord is NOT empty => g_passWord != m_passWord
		svn_auth_set_parameter(ab, SVN_AUTH_PARAM_DEFAULT_PASSWORD, g_passWord.c_str()); // this might have been filled by prompt()

	//svn_auth_set_parameter(ab, SVN_AUTH_CRED_SSL_CLIENT_CERT, 

	ctx->auth_baton = ab;

	ctx->notify_func  = 0;//Notify::notify;
	ctx->notify_baton = 0;//m_notify;
	
	ctx->notify_func2  = ClientUtil::NotifyHelp::notify2;
	ctx->notify_baton2 = m_notify2;
	
	// a callback to be used to see if we (the client) wish to cancel the running operation
	// client's cancelOperation() public method allows users to ask for cancel

	ctx->cancel_func  = ClientUtil::OtherHelp::checkCancel;
	ctx->cancel_baton = this;

	m_cancelOperation = false;

	// 1.2 logic with log_msg
	//ctx->log_msg_func = getCommitMessage;                    // used by commit
	//ctx->log_msg_baton = getCommitMessageBaton(message);     // ditto

	// 1.3 logic?
	// if not filled (is NULL) means that Subversion should try log_msg_func
	ctx->log_msg_baton2 = ClientUtil::CommitHelp::createLogMsgBaton(p_strMessage, m_commitMessageBuilder, pool);
	ctx->log_msg_func2  = ClientUtil::CommitHelp::logMsg;

	ctx->log_msg_baton  = 0; // safety 1st
	ctx->log_msg_func   = 0;

	const char *configDir = m_configDir.c_str();
	if(m_configDir.length() == 0)
		configDir = NULL;
	if (( err = 
		svn_config_get_config (&(ctx->config), configDir, pool)))
	{
		Util::handleSVNError(err);
		return NULL;
	}

	// no progress feedback yet
	ctx->progress_func  = 0;
	ctx->progress_baton = 0;


	// Use custom ssh client "GMEplink" (no console window)
	CString appPath;
	::GetModuleFileName(NULL, appPath.GetBuffer(MAX_PATH), MAX_PATH);
	appPath.ReleaseBuffer();
	appPath = appPath.Left(appPath.ReverseFind('\\')+1);
	svn_config_t * cfg = (svn_config_t *)apr_hash_get (ctx->config, SVN_CONFIG_CATEGORY_CONFIG,
			APR_HASH_KEY_STRING);
	const char * sshValue = NULL;
	svn_config_get(cfg, &sshValue, SVN_CONFIG_SECTION_TUNNELS, "ssh", "");
	if ((sshValue == NULL)||(sshValue[0] == 0)) {
		CString tsvn_ssh = _T("\"") + appPath + _T("GMEplink.exe") + _T("\"");
		tsvn_ssh.Replace('\\', '/');
		svn_config_set(cfg, SVN_CONFIG_SECTION_TUNNELS, "ssh", tsvn_ssh);
	}

	svn_config_set(cfg, SVN_CONFIG_SECTION_MISCELLANY, SVN_CONFIG_OPTION_ENABLE_AUTO_PROPS, false);

	return ctx;
}

void ClientUtil::CommitHelp::MsgBuilder::dealWithItem( std::string& res, svn_client_commit_item_t * item)
{
	//const char *      item->path
	//svn_node_kind_t   item->kind;
	//const char *      item->url;
	//const char *      item->copyfrom_url;
	//svn_revnum_t      item->revision;
	if( !item)
	{
		res += "<Null item>\n";
		return;
	}

	if(item->state_flags & SVN_CLIENT_COMMIT_ITEM_ADD)
		res += "[Add]";
	if(item->state_flags & SVN_CLIENT_COMMIT_ITEM_DELETE)
		res += "[Del]";
	if(item->state_flags & SVN_CLIENT_COMMIT_ITEM_TEXT_MODS)
		res += "[Mod]";
	if(item->state_flags & SVN_CLIENT_COMMIT_ITEM_PROP_MODS)
		res += "[Pro]";
	if(item->state_flags & SVN_CLIENT_COMMIT_ITEM_IS_COPY)
	{
		res += "[Cop]";
		res += "From: "; res += ClientUtil::charstar2str( item->copyfrom_url, "CopyFromURL");
	}

	char buff[32]; itoa( item->revision, &buff[0], 10);
	res += " Rev#"; res += buff; 
	res += " "; 
	res += ClientUtil::charstar2str( item->path, "Path");
	res += "\n";
}

std::string ClientUtil::CommitHelp::MsgBuilder::build(const apr_array_header_t *commit_items)
{
	std::string m;
	if( !commit_items) return m;

	int count = commit_items->nelts;
	for(int i = 0; i < count; i++)
	{
		// get the commit item
		svn_client_commit_item_t *item
			= ((svn_client_commit_item_t **) commit_items->elts)[i];

		dealWithItem( m, item);
	}

	return m;
}

/** Callback type used by commit-y operations to get a commit log message
 * from the caller.
 *  
 * Set @a *log_msg to the log message for the commit, allocated in @a 
 * pool, or @c NULL if wish to abort the commit process.  Set @a *tmp_file 
 * to the path of any temporary file which might be holding that log 
 * message, or @c NULL if no such file exists (though, if @a *log_msg is 
 * @c NULL, this value is undefined).  The log message MUST be a UTF8 
 * string with LF line separators.
 *
 * @a commit_items is a read-only array of @c svn_client_commit_item2_t
 * structures, which may be fully or only partially filled-in,
 * depending on the type of commit operation.
 *
 * @a baton is provided along with the callback for use by the handler.
 *
 * All allocations should be performed in @a pool.
 *
 * @since New in 1.3.
 */

// static, callback method
svn_error_t * ClientUtil::CommitHelp::logMsg(const char **log_msg,
										const char **tmp_file,
										const apr_array_header_t *commit_items,
										void *baton,
										apr_pool_t *pool)
{
	*log_msg = NULL;
	*tmp_file = NULL;
	LogMsgBaton *lmb = (LogMsgBaton *) baton; // convert the baton to the specific structure we passed in

	// is there a message composer?
	if (lmb && lmb->m_messageHandler)
	{
		// build special message based on the to-be-commited-items array
		std::string spec_msg = lmb->m_messageHandler->build( commit_items);
		*log_msg = apr_pstrdup( pool, spec_msg.c_str()); 
		return SVN_NO_ERROR;
	}
	else if (lmb && lmb->m_message) // or at least a message?
	{
		*log_msg = apr_pstrdup (pool, lmb->m_message);
		return SVN_NO_ERROR;
	}

	return SVN_NO_ERROR;
}

// static 
void * ClientUtil::CommitHelp::createLogMsgBaton( const char * p_strMessage, CommitHelp::MsgBuilder * p_msgComposer, apr_pool_t *pool)
{
	// the baton created by this method will affect 
	// how CommitHelp::logMsg() will deal with the 
	// array of commit_items

	if( p_strMessage != NULL || p_msgComposer)
	{
		LogMsgBaton *baton = (LogMsgBaton *) apr_palloc (pool, sizeof (*baton));

		baton->m_message = p_strMessage; // beware: ptr assignment only !!
		baton->m_messageHandler = p_msgComposer;

		return baton;
	}
	//return NULL;
	
	// create a baton with dummy string value for empty p_strMessages as well:
	static const char * def_str = "Default";
	LogMsgBaton *baton = (LogMsgBaton *) apr_palloc (pool, sizeof (*baton));
	baton->m_message = def_str; // beware: ptr assignment only !!
	return baton;
}

void Client::cancelOperation()
{
	m_cancelOperation = true;
}

// static, callback method
svn_error_t * ClientUtil::OtherHelp::checkCancel(void *cancelBaton)
{
	Client *that = (Client*)cancelBaton;
	if(that->m_cancelOperation)
		return svn_error_create (SVN_ERR_CANCELLED, NULL,
		"Operation canceled");
	else
		return SVN_NO_ERROR;
}

// static, callback method, conforms to svn_wc_status_func2_t
void ClientUtil::StatusHelp::statusReceiver( void *baton, const char *path, 
		svn_wc_status2_t *status)
{
	StatusBaton *status_baton = (StatusBaton*) baton;
	StatusEntry status_entry;
	status_entry.m_path = apr_pstrdup( status_baton->m_pool, path);
	status_entry.m_status = svn_wc_dup_status2( status, status_baton->m_pool);
	status_baton->m_statusVect.push_back( status_entry);
}

/**
* notification function passed as svn_wc_notify_func2_t
* @param baton notification instance is passed using this parameter
* @param notify all the information about the event
* @param pool an apr pool to allocated memory
*/

void ClientUtil::NotifyHelp::notify2( void *baton,
								const svn_wc_notify_t *notify,
								apr_pool_t *pool)
{
	// Notify the world that @a notify->action has happened to @a notify->path.

	// a NotifyHelp object is used as the baton
	NotifyHelp * that = (NotifyHelp *) baton;
	if( that) // sanity check
	{
		// call our method
		that->onNotify( notify, pool);
	}
}

// we deal with the notification that the @a notify->action has happened to @a notify->path.
void ClientUtil::NotifyHelp::onNotify( const svn_wc_notify_t *notify, apr_pool_t *pool)
{
	if (notify->action == svn_wc_notify_failed_lock) {
		char errbuff[BUFSIZ];
		const char* errbuff2 = svn_err_best_message(notify->err, errbuff, BUFSIZ);
		m_msg.append(errbuff2 ? errbuff2 : errbuff);
		m_msg.append("<br>"); // This is a terrible hack
		m_OK = false;
	}
}

//
bool Client::info2Qck( const char* p_path, bool p_recurse, ClientUtil::InfoHelp::InfoVec& p_infoVect, bool p_suppressErrorMsg)
{
	return sub_info2( p_path, Revision(), Revision(), p_recurse, p_infoVect, p_suppressErrorMsg);
}

bool Client::sub_info2(const char *path, Revision &revision, Revision &pegRevision, bool recurse, ClientUtil::InfoHelp::InfoVec& p_infoVect, bool p_suppressIllegalUrlErrorMsg)
{
	ClientUtil::InfoHelp::InfoBaton info_baton;
	Pool reqPool;

	if(path == NULL)
	{
		Util::throwNullPointerException("path");
		return false;
	}

	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	if(ctx == NULL)
	{
		return false;
	}
	Path checkedPath(path, reqPool.pool());
	svn_error_t *Err = checkedPath.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return false;
	}

	info_baton.m_pool = reqPool.pool();

	Err = svn_client_info (
		checkedPath.c_str(), 
		pegRevision.revision(),
		revision.revision(),
		ClientUtil::InfoHelp::infoReceiver, // callback function 
		&info_baton,             // callback function's input baton parameter
		recurse ? TRUE :FALSE,
		ctx,
		reqPool.pool());

	if (Err == NULL)
	{
		p_infoVect = info_baton.m_infoVect; // copy the vector, could also use apr specific copier?
		return true;
	}
	else
	{
		// suppress SVN_ERR_RA_ILLEGAL_URL and SVN_ERR_ENTRY_MISSING_URL only, if allowed by p_suppressIllegalUrlErrorMsg
		if( !p_suppressIllegalUrlErrorMsg ||
			p_suppressIllegalUrlErrorMsg && Err->apr_err != SVN_ERR_RA_ILLEGAL_URL && Err->apr_err != SVN_ERR_ENTRY_MISSING_URL)
				Util::handleSVNError(Err);
		return false;
	}
}

// static, callback method
svn_error_t *ClientUtil::InfoHelp::infoReceiver(void *baton, 
								  const char *path,
								  const svn_info_t *info,
								  apr_pool_t *pool)
{
	// we don't create here java Status object as we don't want too many local 
	// references
	InfoBaton *info_baton = (InfoBaton*) baton; // convert the baton to the specific structure we passed in
	InfoEntry info_entry;
	info_entry.m_path = apr_pstrdup( info_baton->m_pool, path);
	info_entry.m_info = (svn_info_t*) apr_pcalloc ( info_baton->m_pool, sizeof(svn_info_t));
	info_entry.m_info->URL = apr_pstrdup( info_baton->m_pool,info->URL);
	info_entry.m_info->rev = info->rev;
	info_entry.m_info->kind = info->kind;
	info_entry.m_info->repos_root_URL = apr_pstrdup(info_baton->m_pool, 
		info->repos_root_URL);
	info_entry.m_info->repos_UUID = apr_pstrdup( info_baton->m_pool, info->repos_UUID);
	info_entry.m_info->last_changed_rev = info->last_changed_rev;
	info_entry.m_info->last_changed_date = info->last_changed_date;
	info_entry.m_info->last_changed_author = apr_pstrdup(info_baton->m_pool, 
		info->last_changed_author);
	if(info->lock != NULL)
		info_entry.m_info->lock = svn_lock_dup( info->lock, info_baton->m_pool);
	else
		info_entry.m_info->lock = NULL;
	info_entry.m_info->has_wc_info = info->has_wc_info;
	info_entry.m_info->schedule = info->schedule;
	info_entry.m_info->copyfrom_url = apr_pstrdup( info_baton->m_pool, 
		info->copyfrom_url);
	info_entry.m_info->copyfrom_rev = info->copyfrom_rev;
	info_entry.m_info->text_time = info->text_time;
	info_entry.m_info->prop_time = info->prop_time;
	info_entry.m_info->checksum = apr_pstrdup( info_baton->m_pool, info->checksum);
	info_entry.m_info->conflict_old = apr_pstrdup( info_baton->m_pool, 
		info->conflict_old);
	info_entry.m_info->conflict_new = apr_pstrdup( info_baton->m_pool,
		info->conflict_new);
	info_entry.m_info->conflict_wrk = apr_pstrdup( info_baton->m_pool,
		info->conflict_wrk);
	info_entry.m_info->prejfile = apr_pstrdup( info_baton->m_pool, info->prejfile);

	info_baton->m_infoVect.push_back( info_entry);
	return SVN_NO_ERROR;
}

int Client::sub_add( const char *p_pathOrUrl, bool p_rec)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();

	if(p_pathOrUrl == NULL)
	{
		Util::throwNullPointerException("p_pathOrUrl");
		return -1;
	}

	Path url(p_pathOrUrl, reqPool.pool());

	svn_error_t *Err = url.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}

	svn_client_ctx_t *ctx = getContext(NULL, reqPool.pool());
	if(ctx == NULL)
	{
		return -1;
	}

	Err = svn_client_add3 ( url.c_str(),
		p_rec, // recursive
		true,  //force
		true,  // no ignore
		ctx,
		apr_pool);

	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	return 1;
}


long Client::sub_mkdir(Targets &targets, const char * p_msg)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();
	svn_client_commit_info_t *commit_info = NULL;
	// 
	// svn_client_mkdir takes svn_client_commit_info_t
	// svn_client_mkdir2 takes svn_commit_info_t
	//
	const apr_array_header_t *targets2 = targets.array(reqPool);
	svn_error_t *Err = targets.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	svn_client_ctx_t *ctx = getContext(p_msg, reqPool.pool());
	if(ctx == NULL)
	{
		return -1;
	}

	Err = svn_client_mkdir( &commit_info,
		targets2, // paths
		ctx,
		apr_pool);

	if(Err != NULL)
		Util::handleSVNError(Err);

	if(commit_info && SVN_IS_VALID_REVNUM (commit_info->revision))
		return commit_info->revision;

	return -1;
}

long Client::sub_mkdir2( Targets& targets, const char * p_msg)
{
	Pool reqPool;
	apr_pool_t * apr_pool = reqPool.pool();
	svn_commit_info_t        *commit2_info = NULL;

	const apr_array_header_t *targets2 = targets.array(reqPool);
	svn_error_t *Err = targets.error_occured();
	if(Err != NULL)
	{
		Util::handleSVNError(Err);
		return -1;
	}
	svn_client_ctx_t *ctx = getContext(p_msg, reqPool.pool());
	if(ctx == NULL)
	{
		return -1;
	}

	Err = svn_client_mkdir2( &commit2_info,
		targets2, // paths
		ctx,
		apr_pool);

	if(Err != NULL)
		Util::handleSVNError(Err);

	if( commit2_info && SVN_IS_VALID_REVNUM (commit2_info->revision))
		return commit2_info->revision;

	return -1;
}

//
// std::string.append( const char * ) crashes if parameter ptr is 0, that's why this function is useful
//
// static 
std::string ClientUtil::charstar2str( const char* p_charStar, const std::string p_name)
{
	if( p_charStar)
	{
		return std::string( p_charStar);
	}

	return std::string( "<no" + p_name + ">");
}

#endif
