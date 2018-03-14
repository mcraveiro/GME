#pragma once
#include "Client.h"
#include "CallLogger.h"

class HiClient
	: protected Client // implementation inherited from Client
	, public CallLogger
{
public:
	HiClient( const std::string& p_userName, const std::string& p_passWord);
	~HiClient(void);

	void replaceUserName( const std::string& p_userName);


	// public :
	bool isItUs              ( const std::string& p_userName);
	//bool isLocked            ( const std::string& p_path);
	//bool isLockedByUs        ( const std::string& p_path);
	bool isLockedByOthers    ( const std::string& p_path);
	bool isLockedByWhom      ( const std::string& p_path, bool * p_us, bool * p_else);
	////bool isLockedBy          ( const char * p_path, char * p_res, int p_len);
	bool isLockedByUser      ( const std::string& p_path, std::string& p_holder);
	bool isVersioned         ( const std::string& p_path, bool p_isADir = false, bool p_suppressErrorMsg = false);
	bool info                ( const std::string& p_url,  bool p_recursive, bool p_assembleInfoMsg, std::string& p_infoMsg, std::string& p_author, std::string& p_lockOwner);
	bool status              ( const std::string& p_path, bool p_assembleStatusMsg, std::string& p_statMsg);
	bool statusOnServer      ( const std::string& p_path, bool p_assembleStatusMsg, std::string& p_statMsg, bool *p_outOfDate, bool *p_repoEntryModified);
	bool cleanup             ( const std::string& p_path);
	bool resolve             ( const std::string& p_path, bool p_recursive);

	bool tryLock             ( const std::string& p_path);
	bool unLock              ( const std::string& p_path);
	bool unLock              ( const std::vector< std::string> & p_pathVec);
	bool lockableProp        ( const std::string& p_path);
	bool getLatest           ( const std::string& p_path);
	bool lightCheckOut       ( const std::string& p_path, const std::string& p_localDir); // this is used as svn checkout (in contrast with lock+checkout as in other versioning systems)
	bool commitAll           ( const std::string& p_path, const std::string& p_comment, bool p_keepCheckedOut);
	bool add                 ( const std::string& p_path, bool p_recursive);
	bool mkDirOnServer       ( const std::string& p_path);
	bool speedLock			 ( const std::vector< std::string> & targets, std::string &msg);
};

