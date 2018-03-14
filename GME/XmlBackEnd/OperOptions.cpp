#include "stdafx.h"
#include <fstream>
#include "OperOptions.h"
#include "CoreXmlFile.h"

// OperatingOptions
OperatingOptions::OperatingOptions()
: m_defCheckInOnSave( false)
, m_autoCommit(false)
, m_defCheckOutOnAction( false)
, m_alwaysFullLock( false)
, m_onDeleteAlwaysFullLock( false)
, m_onLoadShowStatus( true)
, m_measureTime( false)
, m_createLog( false)
, m_createSvnLog( false)
, m_doRefTargetLock( true)
, m_doConnEndPointLock( true)
, m_doConnSegmentLock( true)
, m_doModelParentLock( true)
, m_doBaseTypeLock( true)
, m_partialLoad( false)
, m_useAccountInfo( false)
, m_automaticLogin( false)
, m_useAPIForSvn( false)
, m_useBulkCommit( true )
, m_dirNamesHashed( false)
, m_dirNamesHashVal( 0)
, m_purgeDelayFactor( 1.)
, m_svnLogFileName()
, m_nameParadigmFile()
, m_defUserName()
, m_defPassword()
, m_prefUrl()
, m_prefAccessMethod()
{
	const char* userprofile = getenv("USERPROFILE");
	if (userprofile)
	{
		loadSettings(userprofile, "GME_MU_config.opt");
	}
}

void OperatingOptions::reset()
{
	m_defCheckInOnSave      = false;
	m_autoCommit = false;
	m_defCheckOutOnAction   = false;
	m_alwaysFullLock        = false;
	m_onDeleteAlwaysFullLock= false;
	m_onLoadShowStatus      = true;
	m_measureTime           = false;
	m_createLog             = false;
	m_createSvnLog          = false;
	m_doRefTargetLock       = true;
	m_doConnEndPointLock    = true;
	m_doConnSegmentLock     = true;
	m_doModelParentLock     = true;
	m_doBaseTypeLock        = true;
	m_partialLoad           = false;
	m_useAccountInfo        = false;
	m_automaticLogin        = false;
	m_useAPIForSvn          = false;
	m_useBulkCommit         = false;
	m_dirNamesHashed        = false;
	m_dirNamesHashVal       = 0;
	m_purgeDelayFactor      = 1.;
	m_svnLogFileName        = "";
	m_nameParadigmFile      = "";
	m_defUserName           = "";
	m_defPassword           = "";
	m_prefUrl               = "";
	m_prefAccessMethod      = "";
}

void OperatingOptions::load( const std::string& p_folder)
{
	reset();
	const char* userprofile = getenv("USERPROFILE");
	if (userprofile)
	{
		loadSettings(userprofile, "GME_MU_config.opt");
	}
	loadSettings( p_folder, m_sysConfName);
	loadSettings( p_folder, m_usrConfName);
}

bool OperatingOptions::parseBool( const std::string& p_line, const std::string& p_optStr)
{
	bool truef  = p_line.find( "true" , p_optStr.length())  != std::string::npos; // look for 'true' after p_optStr.length() offset
	bool falsef = p_line.find( "false", p_optStr.length()) != std::string::npos;
	bool rv = true;

	if( truef && !falsef)
		rv = true;
	else if( !truef && falsef)
		rv = false;
	else if( !truef && !falsef) // value doesn't contain neither true, nor false, assume true
		rv = true;
	else if( truef && falsef) // value might contain 'true' and 'false'
		rv = true;

	return rv;
}

std::string OperatingOptions::parseStr( const std::string& p_line, const std::string& p_optStr)
{
	std::string ret;
	size_t pos = p_line.find_first_not_of( "\t =", p_optStr.length()); // skip whitespaces, equal sign
	if( pos != std::string::npos) // found
	{
		int cpos = p_line.find_first_of( "\t #", pos); // find comments at the end of line, skip whitespaces too
		// value is between pos and cpos
		ret = p_line.substr( pos, cpos - pos); // cpos == -1 case is tolerated too by substr
	}

	return ret;
}

void OperatingOptions::loadSettings( const std::string& p_folder, const std::string& p_fName)
{
	std::ifstream options;
	options.open( ( p_folder + "\\" + p_fName).c_str(), std::ios_base::in);
	if( options.is_open())
	{
		char buff[1024];
		while( options.getline( buff, 1024))
		{
			std::string line( buff);
			size_t valid_cont = line.find( '#');
			if( valid_cont != std::string::npos) // line = line.substr( 0, valid_cont); // cut part behind #
				line.erase( valid_cont, std::string::npos); // cut part behind #
			
			if( line.empty())
				continue;
			else if( 0 == line.find( "DefCheckInOnSave"))//=true"))
			{
				m_defCheckInOnSave = parseBool( line, "DefCheckInOnSave");//m_defCheckInOnSave = true;
			}
			else if( 0 == line.find("AutoCommit"))//=true"))
			{
				m_autoCommit = parseBool(line, "AutoCommit");
			}
			else if( 0 == line.find( "DefCheckOutOnAction"))//=true"))
			{
				m_defCheckOutOnAction = parseBool( line, "DefCheckOutOnAction"); //m_defCheckOutOnAction = true;
			}
			else if( 0 == line.find( "PartialLoad"))//=true"))
			{
				m_partialLoad = parseBool( line, "PartialLoad"); //m_partialLoad = true;
			}
			else if( 0 == line.find( "UseAccountInfo"))//=true"))
			{
				m_useAccountInfo = parseBool( line, "UseAccountInfo"); //m_useAccountInfo = true;
			}
			else if( 0 == line.find( "AutomaticLogin"))//=true"))
			{
				m_automaticLogin = parseBool( line, "AutomaticLogin"); //m_automaticLogin = true;
			}
			else if( 0 == line.find( "account"))
			{
				//m_defUserName = line.substr( std::string( "account=").length());
				m_defUserName = parseStr( line, "account");
			}
			else if( 0 == line.find( "phrase"))
			{
				//m_defPassword = line.substr( std::string( "phrase=").length());
				m_defPassword = parseStr( line, "phrase");
			}
			else if( 0 == line.find( "PreferredUrl"))
			{
				m_prefUrl = parseStr( line, "PreferredUrl");
			}
			//else if( 0 == line.find( "AlwaysFullLock=true"))
			//{
			//	m_alwaysFullLock = true;
			//}
			//else if( 0 == line.find( "OnDeleteAlwaysFullLock=true"))
			//{
			//	m_onDeleteAlwaysFullLock = true;
			//}
			else if( 0 == line.find( "OnLoadShowStatus"))//=false"))
			{
				m_onLoadShowStatus = parseBool( line, "OnLoadShowStatus"); //m_onLoadShowStatus = false;
			}
			else if( 0 == line.find( "MeasureTime"))//=true"))
			{
				m_measureTime = parseBool( line, "MeasureTime"); //m_measureTime = true;
			}
			else if( 0 == line.find( "Log"))//=true"))
			{
				m_createLog = parseBool( line, "Log"); //m_createLog = true;
			}
			else if( 0 == line.find( "DoSvnLog"))
			{
				m_createSvnLog = parseBool( line, "DoSvnLog");
			}
			else if( 0 == line.find( "SvnLogFile"))
			{
				m_svnLogFileName = parseStr( line, "SvnLogFile");
			}
			else if( 0 == line.find( "UseApiForSvn"))
			{
				m_useAPIForSvn = parseBool( line, "UseApiForSvn");
			}
			else if( 0 == line.find( "UseBulkCommit"))
			{
				m_useBulkCommit = parseBool( line, "UseBulkCommit");
			}
			else if( 0 == line.find( "DirNamesHashed"))
			{
				m_dirNamesHashed = parseBool( line, "DirNamesHashed");
			}
			else if( 0 == line.find( "DirNamesHashedVal"))
			{
				int stoi = 0;
				std::string v = parseStr( line, "DirNamesHashedVal");
				if( 1 == sscanf( v.c_str(), "%u", &stoi) && stoi >= 0)
					m_dirNamesHashVal = stoi;
			}
			else if( 0 == line.find( "PurgeDelayed"))
			{
				float stor = 0.;
				std::string v = parseStr( line, "PurgeDelayed");
				if( 1 == sscanf( v.c_str(), "%f", &stor) && stor >= 0)
					m_purgeDelayFactor = stor;
				//const char * ptr = line.c_str();
				//if( 1 == sscanf( ptr + std::string( "PurgeDelayed=").length(), "%f", &stor) && stor >= 0)
				//	m_purgeDelayFactor = stor;
			}
			//else if( 0 == line.find( "DoRefTargetLock=false"))
			//	m_doRefTargetLock = false;
			//else if( 0 == line.find( "DoConnEndPointLock=false"))
			//	m_doConnEndPointLock = false;
			//else if( 0 == line.find( "DoConnSegmentLock=false"))
			//	m_doConnSegmentLock = false;
			//else if( 0 == line.find( "DoModelParentLock=false"))
			//	m_doModelParentLock = false;
			//else if( 0 == line.find( "DoBaseTypeLock=false"))
			//	m_doBaseTypeLock = false;
		}
		options.close();
	}
}

void OperatingOptions::display( CCoreXmlFile * const parent)
{
	parent->sendMsg( std::string( "User options loaded:") , MSG_INFO);
	parent->sendMsg( std::string( "DefCheckInOnSave=" )     + (m_defCheckInOnSave?"true":"false"), MSG_INFO );
	parent->sendMsg( std::string( "DefCheckOutOnAction=" )  + (m_defCheckOutOnAction?"true":"false"), MSG_INFO );
	//parent->sendMsg( std::string( "AlwaysFullLock=" )       + (m_alwaysFullLock?"true":"false"), MSG_INFO);
	//parent->sendMsg( std::string( "OnDeleteAlwaysFullLock=")+ (m_onDeleteAlwaysFullLock?"true":"false"), MSG_INFO);
	parent->sendMsg( std::string( "OnLoadShowStatus=")      + (m_onLoadShowStatus?"true":"false"), MSG_INFO);
	//parent->sendMsg( std::string( "[Debug]MeasureTime=")           + (m_measureTime?"true":"false"), MSG_INFO);
#ifdef _DEBUG
	//parent->sendMsg( std::string( "[DebugMode]Log=")                   + (m_createLog?"true":"false"), MSG_INFO);
#endif
	parent->sendMsg( std::string( "PartialLoad=")           + (m_partialLoad?"true":"false"), MSG_INFO);
	//parent->sendMsg( std::string( "Adv. Settings {RTgt, CEnd, CSeg, ModP, BasT}=") 
	//	+ (m_doRefTargetLock?"{true,":"{false,")
	//	+ (m_doConnEndPointLock?"true,":"false,")
	//	+ (m_doConnSegmentLock?"true,":"false,")
	//	+ (m_doModelParentLock?"true,":"false,")
	//	+ (m_doBaseTypeLock?"true}":"false}"), MSG_INFO);
	char buff[250]; sprintf( buff, "%.2f, equal to %i minutes", m_purgeDelayFactor, (int)(m_purgeDelayFactor * 60));
	CTime back = CTime::GetCurrentTime() - CTimeSpan( 0, 0, (int)(m_purgeDelayFactor * 60), 0);

	parent->sendMsg( std::string( "PurgeDelayed=") + buff, MSG_INFO);
	if( m_purgeDelayFactor == 0.)
		parent->sendMsg( std::string( "Purge events: Disabled."), MSG_INFO);
	else
		parent->sendMsg( std::string( "Purge events older than ") + (LPCTSTR) back.Format( "[%Y-%m-%d %H:%M:%S]."), MSG_INFO);

	if( m_useAccountInfo)
		parent->sendMsg( std::string( m_automaticLogin?"Auto":"Manual") + std::string(" login with account=\"") + m_defUserName + "\"", MSG_INFO);
	if( !m_prefUrl.empty())
		parent->sendMsg( std::string( "PreferredUrl: ") + m_prefUrl, MSG_INFO);

	if( m_useBulkCommit)
		parent->sendMsg( std::string( "Bulk commit will be used"), MSG_INFO);

	// some conf settings are protected
	// thus not publicized too much: "DoSvnLog", "SvnLogFile"
}

