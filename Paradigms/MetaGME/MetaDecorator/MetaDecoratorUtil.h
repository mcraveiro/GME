//################################################################################################
//
// Meta Decorator Utilities
//	MetaDecoratorUtil.h
//
//################################################################################################

#ifndef MetaDecoratorUtil_h
#define MetaDecoratorUtil_h

#pragma warning( disable : 4786 )

#include "StdAfx.h"

#include <vector>
#include <map>

#include "DecoratorStd.h"

namespace MetaDecor {

class MetaDecoratorUtils;

MetaDecoratorUtils& GetDecorUtils();

//################################################################################################
//
// CLASS : MetaDecoratorUtils
//
//################################################################################################

class MetaDecoratorUtils
{
	private :
		std::map<CString,ShapeCode>	m_mapShape;
		std::map<CString,bool>		m_mapFCOs;

	public :
		MetaDecoratorUtils();
		~MetaDecoratorUtils();

		ShapeCode		GetShapeCode(const CString& kindName) const;
		bool			IsFCO(const CString& kindName) const;

};

}; // namespace MetaDecor

#endif // MetaDecoratorUtil_h