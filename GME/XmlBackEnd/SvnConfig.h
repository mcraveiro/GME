#pragma once

#define USESVN  1

#if(USESVN)
//#define USENEON 1	// (GPL licensing!)
#define USESERF 1


#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shfolder.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "psapi.lib")


#pragma comment(lib, "libapr-1.lib")
#pragma comment(lib, "libaprutil-1.lib")
#pragma comment(lib, "libapriconv-1.lib")
#pragma comment(lib, "xml.lib")

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

#pragma comment(lib, "svn_client-1.lib")
#pragma comment(lib, "svn_delta-1.lib")
#pragma comment(lib, "svn_diff-1.lib")
#pragma comment(lib, "svn_fs-1.lib")
#pragma comment(lib, "libsvn_fs_fs-1.lib")
#pragma comment(lib, "libsvn_fs_util-1.lib")
#pragma comment(lib, "svn_ra-1.lib")
#pragma comment(lib, "libsvn_ra_local-1.lib")
#pragma comment(lib, "libsvn_ra_svn-1.lib")
#pragma comment(lib, "svn_repos-1.lib")
#pragma comment(lib, "svn_subr-1.lib")
#pragma comment(lib, "svn_wc-1.lib")

#ifdef _DEBUG
	#pragma comment(lib, "zlibstatD.lib")
#else
	#pragma comment(lib, "zlibstat.lib")
#endif


#if(USENEON)
	#pragma comment(lib, "libsvn_ra_neon-1.lib")
	#ifdef _DEBUG
		#pragma comment(lib, "libneonD.lib")
	#else
		#pragma comment(lib, "libneon.lib")
	#endif
#endif

#if(USESERF)
	#pragma comment(lib, "libsvn_ra_serf-1.lib")
	#pragma comment(lib, "serf-1.lib")
#endif

#endif