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

#include "Prompter.h"
#include "Pool.h"
#include "svn_client.h"
#include "conio.h"
#include "Util.h"
#include "..\\CredentialDlg.h"
#include "..\\CertificateDlg.h"
#include <iostream> // remove this later

extern std::string g_userName;
extern std::string g_passWord;

//static
void PromptImpl::scanfBlind( std::string& ret)
{
	ASSERT(0);
	int k = 0;
	ret.clear();

	// blind scanf like:
	while( 13 != ( k = getch()) && 10 != k)
	{
		if( k != 8)
		{
			std::cout << "*";
			ret += (char) k;
		}
		else
		{
			ret = ret.substr( 0, ret.length() - 1); // remove last one
			std::cout << "Backspace, please continue typing " << std::endl << std::string( ret.length(), '*');
		}
	}
	if( k == 13 || k == 10)
		std::cout << std::endl;
}

GPromptImpl::GPromptImpl()
: m_pWord( "-")
, m_uName( "-")
, m_credsFilled( false)
{
}

GPromptImpl::~GPromptImpl()
{
}

std::string GPromptImpl::username()
{
	// assumes that prompt() was called before, 
	// and the m_uName field was filled already 
	if( m_credsFilled && m_uName.size() > 0 && m_uName != "-")  // the default value
		return m_uName;

	ASSERT(0);
	CCredentialDlg dlg( true, false, false, m_uName, (const char*) 0);
	if( dlg.DoModal() == IDOK)
	{
		m_uName = dlg.name();
		//m_pWord = dlg.word();
		//m_credsFilled = true;
	}

	return m_uName;
}

std::string GPromptImpl::password()
{
	if( m_credsFilled && m_pWord.size() > 0 && m_pWord != "-")// && m_maySave) // if not the default value ("-") and only if allowed to be saved
		return m_pWord;

	ASSERT(0);
	CCredentialDlg dlg( false, false, false, m_uName, (const char*) 0);
	if( dlg.DoModal() == IDOK)
	{
		m_uName = dlg.name();
		m_pWord = dlg.word();
		m_credsFilled = true;
	}
	return m_pWord;
}

bool GPromptImpl::askYesNo(const char *realm, const char *question, 
                        bool yesIsDefault)
{
	std::string msg = "Realm: " + std::string( realm ? realm:"<norealm>") + "\nQuestion: " + std::string( question ? question :"<noquestion>");
	int res = AfxMessageBox( msg.c_str(), MB_YESNO);
	if( IDYES == res)
		return true;
	else if( IDNO == res)
		return false;
	else
		return yesIsDefault;
}

std::string GPromptImpl::askQuestion(const char *realm, const char *question, 
                                  bool showAnswer, bool maySave)
{
	std::string msg = "Realm: " + std::string( realm ? realm:"<norealm>") + "\nQuestion: " + std::string( question ? question :"<noquestion>");
	if( IDYES == AfxMessageBox( msg.c_str(), MB_YESNO))
		m_answer = "y";
	else
		m_answer = "n";

	m_maySave = false;
	msg = "May save the answer?";
	if( IDYES == AfxMessageBox( msg.c_str(), MB_YESNO))
		m_maySave = true;

	return m_answer;
}

int GPromptImpl::askTrust(const char *question, bool maySave)
{
	std::string q = question; 
	int offs = 0; // make it Windows multiline: \r\n instead of \n
	while( (offs = q.find( '\n', offs + 1)) != -1)
	{
		q.insert( offs, "\r"); offs += 2;
	}
	//if(maySave)
	//{
	//	q += "(R)eject, accept (t)emporarily or accept (p)ermanently?";
	//}
	//else
	//{
	//	q += "(R)eject or accept (t)emporarily?";
	//}

	CCertificateDlg dlg( q, maySave, 0);
	if( dlg.DoModal() == IDOK)
	{
		CCertificateDlg::Response rp = dlg.getResp();
		if( rp == CCertificateDlg::PermanentAccept)
			return Prompter::AcceptPermanently;
		else if( rp == CCertificateDlg::TemoraryAccept)
			return Prompter::AcceptTemporary;
		else
			return Prompter::Reject;
	}
	return Prompter::Reject;
}

bool GPromptImpl::prompt(const char *p_realm, const char *p_username, bool p_maySave)
{
	std::string u_name( p_username?p_username: "<<nousername>>");
	//std::string msg = "Realm: \t'" + std::string( p_realm?p_realm:"<<norealm>>") + "'\nUser: \t'" + u_name + "'\nChange user credentials?";
	//if( IDYES == AfxMessageBox( msg.c_str(), MB_YESNO))
	if( 1)
	{
		CCredentialDlg dlg( false, true, p_maySave, u_name, p_realm);
		if( dlg.DoModal() == IDOK)
		{
			m_uName = dlg.name();
			m_pWord = dlg.word();
			m_credsFilled = true;
			if( p_maySave)
			{
				m_maySave = dlg.maySave();
				if( m_maySave)
				{
					g_userName = m_uName;
					g_passWord = m_pWord;
					// todo: then save it by using the special set_password
					//
				}
			}
			else
				m_maySave = false;
		}
	}
	else // User selected not to change user credentials
	{
		m_uName = p_username;
		//m_pWord = "-";
	}

	return true;
}


//#include "svn_private_config.h"
TextPromptImpl::TextPromptImpl()
: m_pWord( "-")
, m_uName( "-")
{
}

std::string TextPromptImpl::readcnf( const EntryType& p_type)
{
	if( p_type == UserName)
	{
		return "username";
	}
	else if( p_type == Password)
	{
		return m_pWord.size() > 0 ? m_pWord : "<<no pw?>>";
	}
	return "";
}

TextPromptImpl::~TextPromptImpl()
{
}

std::string TextPromptImpl::username()
{
	// assumes that prompt() was called before, 
	// and the m_uName field was filled already 

	if( m_uName.size() > 0 && m_uName != "-")  // the default value
		return m_uName;

	return readcnf( UserName);
}

std::string TextPromptImpl::password()
{
	if( m_pWord.size() > 0 && m_pWord != "-" && m_maySave) // if not the default value ("-") and only if allowed to be saved
		return m_pWord;

	// require it
	std::cout << "Password please : ";
	std::string ret;
	scanfBlind( ret);
	//if( 1 == scanf( "%s", &buff)) // cin eliminates whitespaces
		//ret = buff;
	//ret = readcnf( EntryType::Password);

	if( m_maySave)
	{
		m_pWord = ret;
	}
	return ret;
}

bool TextPromptImpl::askYesNo(const char *realm, const char *question, 
                        bool yesIsDefault)
{
	std::cout << "\nRealm: " << (realm?realm:"<<norealm>>") << "\nQuestion: " << question;
	
	//int k = getch();
	std::string res;
	std::cin >> res;
	int k = res.size() > 0 ? res[0]: ' ';
	if( (char) k == 'n' || (char) k == 'N')
		return false;
	else if( (char) k == 'y' || (char) k == 'Y')
		return true;
	else // if yesIsDefault return true = Yes, if !yesisDefault return false = No
		return yesIsDefault;
}

std::string TextPromptImpl::askQuestion(const char *realm, const char *question, 
                                  bool showAnswer, bool maySave)
{
	std::cout << "\nRealm: " << (realm?realm:"<norealm>") << "\nQuestion: " << (question?question:"<noquestion>");
	std::string ret;
	std::cin >> ret;
	m_answer = ret;
    if(maySave)
        m_maySave = askYesNo(realm, "May save the answer ?", true);
    else
        m_maySave = false;

	return m_answer;
}

int TextPromptImpl::askTrust(const char *question, bool maySave)
{
	std::string q = question;
	if(maySave)
	{
		q += "(R)eject, accept (t)emporarily or accept (p)ermanently?";
	}
	else
	{
		q += "(R)eject or accept (t)emporarily?";
	}
	std::string s_answer = askQuestion( NULL, q.c_str(), true, false);
	char answer = (s_answer.size() > 0)? s_answer[0]: ' ';
	if( answer == 't' || answer == 'T')
	{
		return Prompter::AcceptTemporary;
	}
	else if(maySave && (answer == 'p' || answer == 'P'))
	{
		return Prompter::AcceptPermanently;
	}
	else
		return Prompter::Reject;
}

bool TextPromptImpl::prompt(const char *p_realm, const char *p_username, bool p_maySave)
{
	// aren't we too ambitious here
	std::cout << "\nRealm: " << (p_realm?p_realm:"<<norealm>>") << std::endl << "-Username-: '" << (p_username?p_username: "<<zerousernameptr>>") << "'";
	if( askYesNo( p_realm, "Change user name?", false))
	{
		std::cout << "Username please : ";
		std::cin >> m_uName; // what about storing retv somewhere?

		std::cout << "Password for '" << m_uName << "' please : ";
		scanfBlind( m_pWord);

		if( p_maySave)
		{
			m_maySave = askYesNo( p_realm, "May save the answer ?", true);
			if( m_maySave)
			{
				g_userName = m_uName;
				g_passWord = m_pWord;
				// todo: then save it by using the special set_password
				//
			}
		}
		else
			m_maySave = false;

		return true;
	}
	else
		m_uName = p_username;

	return true;
}

//**************************************
//*************************************

Prompter::Prompter( PromptImpl* p_prompter)
{
    m_impl = p_prompter;
}

Prompter::~Prompter()
{
    if( m_impl)
	{
		delete m_impl;
	}
}

Prompter* Prompter::makePrompter( PromptImpl* p_promptImpl)
{
    return new Prompter( p_promptImpl);
}

svn_auth_provider_object_t*   Prompter::getProviderSimple(apr_pool_t *pool)
{
    svn_auth_provider_object_t *provider;
    svn_client_get_simple_prompt_provider (&provider,
                                           CallbackHelpers::simple_prompt, /* prompt function */
                                           m_impl,                         /* prompt baton    */
                                           2, /* retry limit */
                                           pool);

    return provider;
}

svn_auth_provider_object_t*   Prompter::getProviderUsername(apr_pool_t *pool)
{
    svn_auth_provider_object_t *provider;
    svn_client_get_username_prompt_provider (&provider,
                                             CallbackHelpers::username_prompt, /* prompt function */
                                             m_impl,                           /* prompt baton    */
                                             2, /* retry limit */
                                             pool);

    return provider;
}

svn_auth_provider_object_t*   Prompter::getProviderServerSSLTrust(apr_pool_t *pool)
{
    svn_auth_provider_object_t *provider;
    svn_client_get_ssl_server_trust_prompt_provider
		(&provider, CallbackHelpers::ssl_server_trust_prompt, m_impl, pool);

    return provider;
}

svn_auth_provider_object_t*   Prompter::getProviderClientSSL(apr_pool_t *pool)
{
    svn_auth_provider_object_t *provider;
    svn_client_get_ssl_client_cert_prompt_provider
          (&provider, CallbackHelpers::ssl_client_cert_prompt, m_impl, 2, /* retry limit */pool);

    return provider;
}

svn_auth_provider_object_t*   Prompter::getProviderClientSSLPassword(apr_pool_t *pool)
{
    svn_auth_provider_object_t *provider;
    svn_client_get_ssl_client_cert_pw_prompt_provider
          (&provider, CallbackHelpers::ssl_client_cert_pw_prompt, m_impl, 2 /* retry limit */,
                                                         pool);

    return provider;
}

// *********************************************************************************************
// *********************** C A L L B A C K   H E L P E R   M E T H O D S ***********************
// *********************************************************************************************

// static, callback
svn_error_t *Prompter::CallbackHelpers::simple_prompt(svn_auth_cred_simple_t **cred_p, 
                                     void *baton,
                                     const char *realm, const char *username, 
                                     svn_boolean_t may_save,
                                     apr_pool_t *pool)
{
	// SVN_AUTH_CRED_SIMPLE [svn.simple] credentials ( svn_auth_cred_simple_t ) comprise of
	// const char *username;
	// const char *password;
	// svn_boolean_t may_save;
	// This latter indicates if the credentials may be saved (to disk). 
	// For example, a GUI prompt implementation with a remember password
	// checkbox shall set may_save to TRUE if the checkbox is checked.

	PromptImpl *that = (PromptImpl*)baton;
	svn_auth_cred_simple_t *ret = (svn_auth_cred_simple_t*)apr_pcalloc(pool, sizeof(*ret));

	// show dlg
	if(!that->prompt( realm, username, may_save ? true : false))
		return svn_error_create( SVN_ERR_RA_NOT_AUTHORIZED, NULL, "User canceled dialog");

	// retrieve uname
	std::string user = that->username();
	if(user == "")
		return svn_error_create( SVN_ERR_RA_NOT_AUTHORIZED, NULL, "User canceled dialog");

	// duplicate uname into return variable
	ret->username = apr_pstrdup( pool, user.c_str());

	// retrieve passwrd
	std::string pass = that->password();
	if(pass == "")
		return svn_error_create( SVN_ERR_RA_NOT_AUTHORIZED, NULL, "User canceled dialog");

	// duplicate password into return variable
	ret->password  = apr_pstrdup( pool, pass.c_str());

	// retrieve may_save option
	ret->may_save = that->m_maySave;

	*cred_p = ret;
	return SVN_NO_ERROR;
}

// static, callback
svn_error_t*   Prompter::CallbackHelpers::username_prompt(svn_auth_cred_username_t **cred_p, 
                                       void *baton,
                                       const char *realm, 
                                       svn_boolean_t may_save, 
                                       apr_pool_t *pool)
{
	// SVN_AUTH_CRED_USERNAME [svn.username] credentials ( svn_auth_cred_username_t ) comprise of
	// const char *username;
	// svn_boolean_t may_save;
	// This latter indicates if the credentials may be saved (to disk). 
	// For example, a GUI prompt implementation with a remember username
	// checkbox shall set may_save to TRUE if the checkbox is checked.

	PromptImpl *that = (PromptImpl*)baton;
	svn_auth_cred_username_t *ret = (svn_auth_cred_username_t*)apr_pcalloc(pool, sizeof(*ret));

	// show question
	std::string user = that->askQuestion( realm, "[svn.username] Username: ", true, may_save ? true : false);
	if(user == "")
		return svn_error_create( SVN_ERR_RA_NOT_AUTHORIZED, NULL, "User canceled dialog");

	// duplicate uname into return variable
	ret->username = apr_pstrdup(pool,user.c_str());

	// retrieve may_save option
	ret->may_save = that->m_maySave;

	*cred_p = ret;
	return SVN_NO_ERROR;
}

// static, callback
svn_error_t*   Prompter::CallbackHelpers::ssl_server_trust_prompt(
                              svn_auth_cred_ssl_server_trust_t **cred_p,
                              void *baton,
                              const char *realm,
                              apr_uint32_t failures,
                              const svn_auth_ssl_server_cert_info_t *cert_info,
                              svn_boolean_t may_save,
                               apr_pool_t *pool)
{
	PromptImpl *that = (PromptImpl*)baton;
	svn_auth_cred_ssl_server_trust_t *ret = (svn_auth_cred_ssl_server_trust_t*)apr_pcalloc(pool, sizeof(*ret));

	std::string question = "Error validating server certificate for ";
	question += realm;
	question += ":\n";

	if(failures & SVN_AUTH_SSL_UNKNOWNCA)
	{
		question += " - Unknown certificate issuer\n";
		question += "   Fingerprint: ";
		question += cert_info->fingerprint;
		question += "\n";
		question += "   Distinguished name: ";
		question += cert_info->issuer_dname;
		question += "\n";
	}

	if(failures & SVN_AUTH_SSL_CNMISMATCH)
	{
		question += " - Hostname mismatch (";
		question += cert_info->hostname;
		question += ")\n";
	}

	if(failures & SVN_AUTH_SSL_NOTYETVALID)
	{
		question += " - Certificate is not yet valid\n";
		question += "   Valid from ";
		question += cert_info->valid_from;
		question += "\n";
	}

	if(failures & SVN_AUTH_SSL_EXPIRED)
	{
		question += " - Certificate is expired\n";
		question += "   Valid until ";
		question += cert_info->valid_until;
		question += "\n";
	}

	switch( that->askTrust( question.c_str(), may_save ? true : false))
	{
	case AcceptTemporary:
		*cred_p = ret;
		ret->may_save = FALSE;
		break;
	case AcceptPermanently:
		*cred_p = ret;
		ret->may_save = TRUE;
		ret->accepted_failures = failures;
		break;
	default:
		*cred_p = NULL;
	}
	return SVN_NO_ERROR;
}

// static, callback
svn_error_t*   Prompter::CallbackHelpers::ssl_client_cert_prompt(
                                     svn_auth_cred_ssl_client_cert_t **cred_p,
                                     void *baton, 
                                     const char *realm, 
                                     svn_boolean_t may_save,
                                     apr_pool_t *pool)
{
	PromptImpl *that = (PromptImpl*)baton;
	svn_auth_cred_ssl_client_cert_t *ret = (svn_auth_cred_ssl_client_cert_t*)apr_pcalloc(pool, sizeof(*ret));

	std::string cert_file = that->askQuestion( realm, "client certificate filename: ", true, may_save ? true : false);
	if(cert_file == "")
		return svn_error_create( SVN_ERR_RA_NOT_AUTHORIZED, NULL, "User canceled dialog");

	ret->cert_file = apr_pstrdup(pool, cert_file.c_str());
	ret->may_save = that->m_maySave;
	*cred_p = ret;

	return SVN_NO_ERROR;
}

// static, callback
svn_error_t*   Prompter::CallbackHelpers::ssl_client_cert_pw_prompt(
                                  svn_auth_cred_ssl_client_cert_pw_t **cred_p,
                                  void *baton, 
                                  const char *realm, 
                                  svn_boolean_t may_save,
                                  apr_pool_t *pool)
{
	PromptImpl *that = (PromptImpl*)baton;
	svn_auth_cred_ssl_client_cert_pw_t *ret = (svn_auth_cred_ssl_client_cert_pw_t*)apr_pcalloc(pool, sizeof(*ret));

	std::string info = that->askQuestion( realm, "client certificate passphrase: ", false, may_save ? true : false);
	if(info == "")
		return svn_error_create( SVN_ERR_RA_NOT_AUTHORIZED, NULL, "User canceled dialog");

	ret->password = apr_pstrdup( pool, info.c_str());
	ret->may_save = that->m_maySave;
	*cred_p = ret;

	return SVN_NO_ERROR;
}

// *********************************************************************************************
//  E O F
// *********************************************************************************************

#endif