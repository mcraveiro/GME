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

#if !defined(AFX_PROMPTER_H__6833BB77_DDCC_4BF8_A995_5A5CBC48DF4C__INCLUDED_)
#define AFX_PROMPTER_H__6833BB77_DDCC_4BF8_A995_5A5CBC48DF4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "svn_auth.h"
#include <string>

/**
 * An interface to be implemented by subclasses.
 * Callback methods of Prompter::CallbackHelpers will call these methods if needed.
 */

class PromptImpl
{
public:
	PromptImpl() : m_maySave( false) { }
	virtual ~PromptImpl() { }
	
	virtual bool prompt
		(const char *realm, const char *pi_username, bool maySave) = 0;

	virtual bool askYesNo
		(const char *realm, const char *question, bool yesIsDefault) = 0;
    
    virtual int  askTrust
		(const char *question, bool maySave) = 0;

	virtual std::string askQuestion
		( const char *realm, const char *question, bool showAnswer, bool maySave) = 0;

	virtual std::string password() = 0;
	virtual std::string username() = 0;

    /**
     * flag is the user allowed, that the last answer is stored in 
     * the configuration
     */
    bool m_maySave;


	static void scanfBlind( std::string& ret);
};

class TextPromptImpl : public PromptImpl
{
protected:
	std::string m_uName;
	std::string m_pWord;

	enum EntryType
	{
		UserName
		, Password
	};
	std::string readcnf( const EntryType& p_type);

	std::string m_answer;

public:
	TextPromptImpl();
	virtual ~TextPromptImpl();

	virtual bool prompt(const char *realm, const char *pi_username, bool maySave);

	virtual bool askYesNo(const char *realm, const char *question, bool yesIsDefault);

	virtual int  askTrust(const char *question, bool maySave);

	virtual std::string askQuestion(const char *realm, const char *question, bool showAnswer, bool maySave);

	virtual std::string password();
	virtual std::string username();
};

class GPromptImpl : public PromptImpl
{
protected:
	std::string m_uName;
	std::string m_pWord;
	bool        m_credsFilled;

	std::string m_answer;

public:
	GPromptImpl();
	virtual ~GPromptImpl();

	virtual bool prompt(const char *realm, const char *pi_username, bool maySave);

	virtual bool askYesNo(const char *realm, const char *question, bool yesIsDefault);

	virtual int  askTrust(const char *question, bool maySave);

	virtual std::string askQuestion(const char *realm, const char *question, bool showAnswer, bool maySave);

	virtual std::string password();
	virtual std::string username();
};

/**
 * This class helps setting up the prompters and providers for authentication.
 */
class Prompter
{
	PromptImpl * m_impl;
	Prompter( PromptImpl* p_prompter);

public:
	static Prompter *makePrompter( PromptImpl* p_prompter);

	~Prompter();

	svn_auth_provider_object_t *getProviderUsername(apr_pool_t *pool);
	svn_auth_provider_object_t *getProviderSimple(apr_pool_t *pool);
	svn_auth_provider_object_t *getProviderServerSSLTrust(apr_pool_t *pool);
	svn_auth_provider_object_t *getProviderClientSSL(apr_pool_t *pool);
	svn_auth_provider_object_t *getProviderClientSSLPassword(apr_pool_t *pool);

	enum PrompUserPass
	{
		AcceptPermanently
		, AcceptTemporary
		, Reject
	};

	class CallbackHelpers
	{
	public:
		static svn_error_t*    simple_prompt(
			svn_auth_cred_simple_t **cred_p,
			void *baton, const char *realm,
			const char *username,
			svn_boolean_t may_save,
			apr_pool_t *pool);

		static svn_error_t*    username_prompt(
			svn_auth_cred_username_t **cred_p,
			void *baton,
			const char *realm,
			svn_boolean_t may_save,
			apr_pool_t *pool);

		static svn_error_t*    ssl_server_trust_prompt(
			svn_auth_cred_ssl_server_trust_t **cred_p,
			void *baton,
			const char *realm,
			apr_uint32_t failures,
			const svn_auth_ssl_server_cert_info_t *cert_info,
			svn_boolean_t may_save,
			apr_pool_t *pool);

		static svn_error_t*    ssl_client_cert_prompt(
			svn_auth_cred_ssl_client_cert_t **cred_p,
			void *baton,
			const char *realm,
			svn_boolean_t may_save,
			apr_pool_t *pool);

		static svn_error_t*    ssl_client_cert_pw_prompt(
			svn_auth_cred_ssl_client_cert_pw_t **cred_p,
			void *baton,
			const char *realm,
			svn_boolean_t may_save,
			apr_pool_t *pool);
	};
};

#endif
// !defined(AFX_PROMPTER_H__6833BB77_DDCC_4BF8_A995_5A5CBC48DF4C__INCLUDED_)
