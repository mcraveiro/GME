#include "stdafx.h"
#if(0) // this class is not used for smart copy anymore
#include "MakeKindClosure.h"
#include <algorithm>


//---------------------------------------------------------------------------
//  C  L  A  S  S   CKindSelectClosure
//                  previous enabler of SmartCopy
//---------------------------------------------------------------------------
bool CKindSelectClosure::isValidSelection()
{
	m_peculiarFcos.clear(); // reset to prevent some data left in it
	m_iniSelPath = ""; // not used currently in this class, only for the container
	m_accKind = "";

	checkMeta( m_selFolds, m_selFcos);// works based on the initially selected objects

	getAccKindsInString( m_accKind);

	return true;
}

void CKindSelectClosure::procObj( IMgaObject* obj)
{
	ASSERT( obj != NULL );
	//!m_bIntoLibraries <=> m_iIntoLibraries != 2
	if ( m_iIntoLibraries != LIB_CONT && isInLibrary( obj)) // if no intention to step into libraries and the object is in a library then return
		return;

	objtype_enum objtype;
	COMTHROW( obj->get_ObjType( &objtype) );

	switch( objtype)
	{
	case OBJTYPE_FOLDER:
		{
			if ( m_bFolderContainment)
			{
				CComObjPtr<IMgaFolder> f;
				COMTHROW( ::QueryInterface( obj, f) );
				procFolder( f);
			}
			break;
		}

	case OBJTYPE_MODEL:
		{
			if ( m_bContainment)
			{
				CComObjPtr<IMgaModel> m;
				COMTHROW( ::QueryInterface( obj, m) );

				procModel( m);
			}
			break;
		}

	case OBJTYPE_REFERENCE:
		{
			if ( m_bRefs)
			{
				CComObjPtr<IMgaFCO> fco;
				COMTHROW( ::QueryInterface( obj, fco) );
				m_peculiarFcos.push_back( fco);
			}
			break;
		}

	case OBJTYPE_CONNECTION:
		{
			if ( m_bConns)
			{
				CComObjPtr<IMgaFCO> fco;
				COMTHROW( ::QueryInterface( obj, fco) );
				m_peculiarFcos.push_back( fco);
			}
			break;
		}

	case OBJTYPE_SET:
		{
			if ( m_bSets)
			{
				CComObjPtr<IMgaFCO> fco;
				COMTHROW( ::QueryInterface( obj, fco) );
				m_peculiarFcos.push_back( fco);
			}
			break;
		}

	default: { }
	};
}

bool CKindSelectClosure::postValidateSelection()
{
	return true;
}

void CKindSelectClosure::preProcess()
{
	send2Console("[Smart Copy] Working...");
}

void CKindSelectClosure::postProcess()
{
	// acc kinds already calculated in isValidSelection()
	if( m_peculiarFcos.size() > 0)
		send2Console( "[Smart Copy] The following kinds may accept the copied data: " + m_accKind + ".");
	else //
		send2Console( "[Smart Copy] No data copied.");


	std::sort( m_peculiarFcos.begin(), m_peculiarFcos.end(), ConnsLast());

	m_selFcos = m_topFcos = m_peculiarFcos;
	ASSERT( m_peculiarFolds.size() == 0);
}

//---------------------------------------------------------------------------
//  C  L  A  S  S   CContainerSelectSomeKindClosure
//---------------------------------------------------------------------------
bool CContainerSelectSomeKindClosure::isValidSelection()
{
	m_peculiarFcos.clear(); // reset to prevent some data left in it
	m_iniSelPath = "";

	CComBstrObj kind_name;
	ASSERT( m_selFcos.size() + m_selFolds.size() == 1);
	if( m_selFcos.size() + m_selFolds.size() != 1) return false;

	for( unsigned int i = 0; i < m_selFcos.size(); ++i)
	{
		CComBSTR path;
		COMTHROW( m_selFcos[i]->get_AbsPath( &path));
		CopyTo( path, m_iniSelPath);

		CComObjPtr<IMgaMetaFCO> k_meta;
		COMTHROW( m_selFcos[i]->get_Meta( PutOut( k_meta)));
		COMTHROW( k_meta->get_Name( PutOut(kind_name)) );
	
		ASSERT( i == 0);
	}

	if( i == 0)
	for( unsigned int j = 0; j < m_selFolds.size(); ++j)
	{
		CComBSTR path;
		COMTHROW( m_selFolds[j]->get_AbsPath( &path));
		CopyTo( path, m_iniSelPath);

		CComObjPtr<IMgaMetaFolder> f_meta;
		COMTHROW( m_selFolds[j]->get_MetaFolder( PutOut( f_meta)));
		COMTHROW( f_meta->get_Name( PutOut(kind_name)) );

		ASSERT( j == 0);
	}

	CopyTo( kind_name, m_accKind);// the acc kind is intended to be similar to the one and only container's kind

	return true;
}

void CContainerSelectSomeKindClosure::postProcess()
{
	// m_accKind already calculated in isValidSelection()
	if( m_peculiarFcos.size() > 0)
		send2Console( "[Smart Copy] The following container may accept the copied data: " + m_accKind + ".");
	else
		send2Console( "[Smart Copy] The container does not contain the kinds specified. No data copied.");

	std::sort( m_peculiarFcos.begin(), m_peculiarFcos.end(), ConnsLast());

	m_selFcos = m_topFcos = m_peculiarFcos;
	ASSERT( m_peculiarFolds.size() == 0);
}

bool ConnsLast::operator()( const CComObjPtr<IMgaFCO>& p1, const CComObjPtr<IMgaFCO>& p2) const
{
	// used for sort. 1st rule: Connections will be put in the back of the vector
	// 2nd rule: if the two elements are both connections or none of them is connection then sort on ID
	// returns true if p1 precedes p2

	objtype_enum objtype1, objtype2;
	COMTHROW( p1->get_ObjType( &objtype1));
	COMTHROW( p2->get_ObjType( &objtype2));
	bool c1 = objtype1 == OBJTYPE_CONNECTION;
	bool c2 = objtype2 == OBJTYPE_CONNECTION;
	if( c1 && c2 || !c1 && !c2)
	{
		//return true;// the predicate must be a strict weak ordering, so let's compare the IDs
		CComBstrObj bstr1, bstr2;
		COMTHROW( p1->get_ID( PutOut( bstr1)));
		COMTHROW( p2->get_ID( PutOut( bstr2)));
		return bstr1.Compare( bstr2) < 0;
	}
	else if( c2) // && !c1
		return true;
	else // c1
		return false;
}
#endif
