//################################################################################################
//
// Meta Decorator Utilities
//	MetaDecoratorUtil.cpp
//
//################################################################################################

#include "StdAfx.h"
#include "MetaDecoratorUtil.h"

#include "DecoratorDefs.h"


namespace MetaDecor {

MetaDecoratorUtils metaDecorUtils;

MetaDecoratorUtils& GetDecorUtils()
{
	return metaDecorUtils;
}

MetaDecoratorUtils::MetaDecoratorUtils()
{
	m_mapShape[META_ATOM_KIND]				= CLASS;
	m_mapShape[META_ATOMPROXY_KIND]			= CLASSPROXY;
	m_mapShape[META_MODEL_KIND]				= CLASS;
	m_mapShape[META_MODELPROXY_KIND]		= CLASSPROXY;
	m_mapShape[META_REFERENCE_KIND]			= CLASS;
	m_mapShape[META_REFERENCEPROXY_KIND]	= CLASSPROXY;
	m_mapShape[META_SET_KIND]				= CLASS;
	m_mapShape[META_SETPROXY_KIND]			= CLASSPROXY;
	m_mapShape[META_CONNECTION_KIND]		= CLASS;
	m_mapShape[META_CONNECTIONPROXY_KIND]	= CLASSPROXY;
	m_mapShape[META_FCO_KIND]				= CLASS;
	m_mapShape[META_FCOPROXY_KIND]			= CLASSPROXY;
	m_mapShape[META_FOLDER_KIND]			= CLASS;
	m_mapShape[META_FOLDERPROXY_KIND]		= CLASSPROXY;
	m_mapShape[META_ASPECT_KIND]			= CLASS;
	m_mapShape[META_ASPECTPROXY_KIND]		= CLASSPROXY;
	m_mapShape[META_BOOLEANATTR_KIND]		= CLASS;
	m_mapShape[META_ENUMATTR_KIND]			= CLASS;
	m_mapShape[META_FIELDATTR_KIND]			= CLASS;

	m_mapShape[META_CONSTRAINT_KIND]		= CONSTRAINT;
	m_mapShape[META_CONSTRAINTFUNC_KIND]	= CONSTRAINTFUNC;

	m_mapShape[META_CONNECTOR_KIND]			= CONNECTOR;

	m_mapShape[META_EQUIVALENCE_KIND]		= EQUIVALENCE;
	m_mapShape[META_SAMEFOLDER_KIND]		= EQUIVALENCE;
	m_mapShape[META_SAMEASPECT_KIND]		= EQUIVALENCE;

	m_mapShape[META_INHERITANCE_KIND]		= INHERITANCE;
	m_mapShape[META_IMPINHERITANCE_KIND]	= IMPINHERITANCE;
	m_mapShape[META_INTINHERITANCE_KIND]	= INTINHERITANCE;


	m_mapFCOs[META_ATOM_KIND]				= true;
	m_mapFCOs[META_ATOMPROXY_KIND]			= true;
	m_mapFCOs[META_MODEL_KIND]				= true;
	m_mapFCOs[META_MODELPROXY_KIND]			= true;
	m_mapFCOs[META_REFERENCE_KIND]			= true;
	m_mapFCOs[META_REFERENCEPROXY_KIND]		= true;
	m_mapFCOs[META_SET_KIND]				= true;
	m_mapFCOs[META_SETPROXY_KIND]			= true;
	m_mapFCOs[META_CONNECTION_KIND]			= true;
	m_mapFCOs[META_CONNECTIONPROXY_KIND]	= true;
	m_mapFCOs[META_FCO_KIND]				= true;
	m_mapFCOs[META_FCOPROXY_KIND]			= true;
}

MetaDecoratorUtils::~MetaDecoratorUtils()
{
}

ShapeCode MetaDecoratorUtils::GetShapeCode(const CString& kindName) const
{
	std::map<CString,ShapeCode>::const_iterator it = m_mapShape.find(kindName);
	return (it == m_mapShape.end()) ? NULLSHAPE : it->second;
}

bool MetaDecoratorUtils::IsFCO(const CString& kindName) const
{
	std::map<CString,bool>::const_iterator it = m_mapFCOs.find(kindName);
	return (it == m_mapFCOs.end()) ? false : true;
}

}; // namespace MetaDecor