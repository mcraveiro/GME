
#if !defined(_GMEOLEERROR_H_)
#define _GMEOLEERROR_H_

#define ID_ERROR_FIRST				100
#define ID_ERROR_PROJECT_NOT_OPEN	(ID_ERROR_FIRST + 1)
#define ID_ERROR_MGAPROJECT_NOT_OPEN (ID_ERROR_FIRST + 2)
#define ID_ERROR_PROJECT_OPEN		(ID_ERROR_FIRST + 3)
#define ID_ERROR_NO_CONSTMGR		(ID_ERROR_FIRST + 4)
#define ID_ERROR_MODEL_INVALID		(ID_ERROR_FIRST + 5)
#define ID_ERROR_ASPECT_INVALID		(ID_ERROR_FIRST + 6)
#define ID_ERROR_PANEL_INVALID		(ID_ERROR_FIRST + 7)
#define ID_ERROR_PTR_INVALID		(ID_ERROR_FIRST + 8)
#define ID_ERROR_NOT_FOUND			(ID_ERROR_FIRST + 9)

#define PRECONDITION_ACTIVE_PROJECT \
	if (theApp.guiMetaProject == NULL) { \
		AfxThrowOleDispatchException(ID_ERROR_PROJECT_NOT_OPEN, \
			_T("GUI has no active MGA project.")); \
	} 

#define PRECONDITION_ACTIVE_REAL_MGA_PROJECT	\
	if ((theApp.mgaProject == NULL) || (!theApp.proj_type_is_mga)) { \
		AfxThrowOleDispatchException(ID_ERROR_MGAPROJECT_NOT_OPEN, \
			_T("GUI has no active (valid) MGA project.")); \
	}

#define PRECONDITION_NO_PROJECT \
if (theApp.guiMetaProject != NULL) { \
		AfxThrowOleDispatchException(ID_ERROR_PROJECT_OPEN, \
			_T("GUI has active MGA project.")); \
	}

#define PRECONDITION_ACTIVE_CONSTMGR \
	if (theApp.mgaConstMgr == NULL) { \
		AfxThrowOleDispatchException(ID_ERROR_NO_CONSTMGR, \
			_T("GUI has no active Constraint Manager.")); \
	}

#define PRECONDITION_VALID_MODEL \
	if (!AmIValid()) { \
		AfxThrowOleDispatchException(ID_ERROR_MODEL_INVALID, \
			_T("Model window is invalid. Probably it is closed.")); \
	} 

#define PRECONDITION_VALID_ASPECT \
	if (!AmIValid()) { \
		AfxThrowOleDispatchException(ID_ERROR_ASPECT_INVALID, \
			_T("Aspect is invalid. Probably its model window is closed.")); \
	} 

#define PRECONDITION_VALID_PANEL \
	if ((!m_control) || (!m_control->GetSafeHwnd())) { \
		AfxThrowOleDispatchException(ID_ERROR_PANEL_INVALID, \
			_T("GUI panel is invalid.")); \
	} 

#define PRECONDITION_VALID_PTR( x, y) \
	if ( (!x)) { \
		AfxThrowOleDispatchException(ID_ERROR_PTR_INVALID, \
			_T(y)); \
	} 

#define SHOW_ERROR( z) \
	{ \
		AfxThrowOleDispatchException(ID_ERROR_NOT_FOUND, \
			(z)); \
	} 

#define THROW_IF_NULL( x, y) \
	if ( (!x)) { \
		throw y; \
	} 

#define THROW_IF_BOOL( x, y) \
	if ( (x)) { \
		throw y; \
	} 

#endif // !defined(_GMEOLEERROR_H_)