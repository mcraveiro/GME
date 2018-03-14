#pragma once

#include "svn_client.h"
#include "svn_config.h"
#include "svn_pools.h"

class CSVNClient;

#define SVN_ERROR_MSG_MAX	256

class CSVNError
{
	friend class CSVNClient;
	friend class CSVNFile;

private:
	CSVNError(svn_error_t* e);
	CSVNError(const CSVNError&) {ASSERT(("Copying CVSNError objects are not supported", false));}

public:
	virtual ~CSVNError();
	CString msg() const;

private:
	svn_error_t	*svnError;
};

class CSVNPool
{
	typedef apr_pool_t* apr_pool_ptr;
public:
	CSVNPool(apr_pool_t* parentPool);
	CSVNPool(const CSVNPool&) {ASSERT(("Copying CSVNPool objects are not supported", false));}
	virtual ~CSVNPool();

	operator apr_pool_ptr() const;

private:
	apr_pool_ptr pool;
};

class CSVNFile
{
	friend class CSVNClient;

private:
	CSVNFile(CSVNClient* client, const CString & filePath);

	void updateStatus(bool checkServer = false);

	// Callbacks
	static svn_error_t* cbStatus(void *baton, const char *path, const svn_client_status_t *status, apr_pool_t *scratch_pool);

public:
	virtual ~CSVNFile();

	bool isVersioned();
	bool isTracked();
	bool isOwned();
	bool isLatest();

	bool update();
	bool takeOwnership();
	bool commit();

private:
	CSVNClient *client;
	CString	filePath;
	bool	versioned;
	bool	tracked;
	bool	owned;
	bool	latest;
};


class CSVNClient
{
	friend class CSVNFile;

public:
	CSVNClient();
	virtual ~CSVNClient();

	void initialize();

	CSVNFile* embraceFile(const CString & filePath);
	void forgetFile(CSVNFile* svnFile);

private:
	// Context Callbacks
	static void cbNotify(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool);
	static svn_error_t* cbLog(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool);
	static svn_error_t* cbCancel(void *cancel_baton);
	static void cbProgress(apr_off_t progress, apr_off_t total, void *baton, apr_pool_t *pool);
	static svn_error_t* cbConflict(svn_wc_conflict_result_t **result, const svn_wc_conflict_description2_t *description, void *baton, apr_pool_t *result_pool, apr_pool_t *scratch_pool);

	// Auth Callbacks
	static svn_error_t* cbAuthPlaintextPrompt(svn_boolean_t *may_save_plaintext, const char *realmstring, void *baton, apr_pool_t *pool);
	static svn_error_t* cbAuthPlaintextPassphrasePrompt(svn_boolean_t *may_save_plaintext, const char *realmstring, void *baton, apr_pool_t *pool);
	static svn_error_t* cbAuthSimplePrompt(svn_auth_cred_simple_t **cred, void *baton, const char *realm, const char *username, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* cbAuthUsernamePrompt(svn_auth_cred_username_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* cbAuthSSLServerTrustPrompt(svn_auth_cred_ssl_server_trust_t **cred, void *baton, const char *realm, apr_uint32_t failures, const svn_auth_ssl_server_cert_info_t *cert_info, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* cbAuthSSLClientCertPWPrompt(svn_auth_cred_ssl_client_cert_pw_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool);
	static svn_error_t* cbAuthSSLClientCertPrompt(svn_auth_cred_ssl_client_cert_t **cred, void *baton, const char *realm, svn_boolean_t may_save, apr_pool_t *pool);

private:
	CList<CSVNFile*, CSVNFile*> svnFiles;
	
	bool isInitialized;

	// These are valid only if initialized
	svn_client_ctx_t *ctx;
	apr_pool_t *pool;

	// Internal communication
	bool canceledOperation;
	svn_wc_notify_action_t lastNotifyAction;
};

