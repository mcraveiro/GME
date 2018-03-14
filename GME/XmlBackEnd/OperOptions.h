#pragma once

class CCoreXmlFile;

class OperatingOptions
{
	bool           parseBool   ( const std::string& p_line, const std::string& p_optStr);
	std::string    parseStr    ( const std::string& p_line, const std::string& p_optStr);
	void           loadSettings( const std::string& p_folder, const std::string& p_name);
public:
	OperatingOptions           ();
	void           reset       ();
	void           load        ( const std::string& p_folder);
	void           display     ( CCoreXmlFile * const p_parent);

	// members
	static const char *  m_usrConfName;
	static const char *  m_sysConfName;
	static const char *  m_sysConfDefContentsSvn;
	static const char *  m_sysConfDefContentsPlain;
	bool                 m_defCheckInOnSave;
	bool m_autoCommit;
	bool                 m_defCheckOutOnAction;
	bool                 m_alwaysFullLock;
	bool                 m_onDeleteAlwaysFullLock;
	bool                 m_onLoadShowStatus;
	bool                 m_measureTime;
	bool                 m_createLog;
	bool                 m_createSvnLog;
	bool                 m_doRefTargetLock;
	bool                 m_doConnEndPointLock;
	bool                 m_doConnSegmentLock;
	bool                 m_doModelParentLock;
	bool                 m_doBaseTypeLock;
	bool                 m_partialLoad;
	bool                 m_useAccountInfo;
	bool                 m_automaticLogin;
	bool                 m_useAPIForSvn;
	bool                 m_useBulkCommit;
	bool                 m_dirNamesHashed;
	int                  m_dirNamesHashVal;
	double               m_purgeDelayFactor;
	std::string          m_svnLogFileName;
	std::string          m_nameParadigmFile;
	std::string          m_defUserName;
	std::string          m_defPassword;
	std::string          m_prefUrl;
	std::string          m_prefAccessMethod;
};
