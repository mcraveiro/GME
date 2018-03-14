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

// class mimicing subversion/bindings/java/javahl/native/svnclient.cpp

class ClientUtil
{
public:
	//
	//   ********  Info *********
	//
	struct InfoHelp
	{
		struct InfoEntry
		{
			const char *m_path;
			svn_info_t *m_info;
		};

		typedef std::vector<InfoEntry> InfoVec;
		typedef InfoVec::const_iterator InfoVecConIter;

		struct InfoBaton
		{
			InfoVec     m_infoVect;
			apr_pool_t *m_pool;
		};

		// callback, which will fill an InfoBaton struct (got in void *baton)
		static svn_error_t * infoReceiver(void *baton, 
										const char *path,
										const svn_info_t *info,
										apr_pool_t *pool);

	};
	
	//
	//   ********  Commit related helper *********
	//
	struct CommitHelp
	{
		class MsgBuilder // builds a proper commit message based on entries
		{
			void dealWithItem( std::string& res, svn_client_commit_item_t * item);
		public:
			std::string build( const apr_array_header_t *commit_items);
		};

		struct LogMsgBaton // log_msg_baton
		{
			const char *      m_message;
			MsgBuilder *      m_messageHandler;
		}; 

		// creates the baton class given to the callback
		static void* createLogMsgBaton( const char * p_strMessage, CommitHelp::MsgBuilder * p_msgComposer, apr_pool_t *pool);


		// callback, which is called by commit-y operations
		static svn_error_t * logMsg(const char **log_msg,
									const char **tmp_file,
									const apr_array_header_t *commit_items,
									void *baton,
									apr_pool_t *pool);

	};


	//
	//   ********  Notify : a Commit and Checkout related helper *********
	//
	struct NotifyHelp
	{
		std::string       m_msg;
		bool			  m_OK;

		/**
		* Handler for Subversion notifications.
		*
		* @param notify all the information about the event
		* @param pool an apr pool to allocated memory
		*/
		void onNotify( const svn_wc_notify_t *notify, apr_pool_t *pool);

		/**
		* notification function passed as svn_wc_notify_func2_t
		* @param baton notification instance is passed using this parameter
		* @param notify all the information about the event
		* @param pool an apr pool to allocated memory
		*/

		// callback, which is called during notification
		static void notify2(void *baton,
						const svn_wc_notify_t *notify,
						apr_pool_t *pool);

	};

	// **************************************
	// other callbacks
	// **************************************
	struct OtherHelp
	{
		static svn_error_t * checkCancel(void *cancelBaton);
	};

	//
	//   ********  Status related helper *********
	//
	struct StatusHelp
	{
		struct StatusEntry
		{
			const char *                   m_path;
			svn_wc_status2_t *             m_status;
		};

		struct StatusBaton
		{
			std::vector<StatusEntry>       m_statusVect;
			apr_pool_t *                   m_pool;
		};

		static void statusReceiver(void *baton, const char *path, 
			svn_wc_status2_t *status);
	};

	typedef std::pair<std::string, long> StatusInfo;
	StatusInfo sub_single_status(const char *path, bool onServer);

	typedef StatusHelp::StatusEntry StatusExtInfo;
	typedef std::vector<StatusExtInfo>   StatusExtInfoVec;
	StatusExtInfoVec sub_extended_status( const char * p_path, bool p_onServer);

	// statics
	static std::string charstar2str( const char* p_charStar, const std::string p_name);

};
