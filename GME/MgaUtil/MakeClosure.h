#ifndef MAKE_CLOSURE_H
#define MAKE_CLOSURE_H

#include "Gme.h"

class GmeEqual
{
	public:
		GmeEqual( CComObjPtr<IMgaObject>);
		GmeEqual( CComObjPtr<IMgaFCO>);
		GmeEqual( CComObjPtr<IMgaFolder>);
		GmeEqual( CComObjPtr<IMgaModel>);

		bool operator() ( CComObjPtr<IMgaObject>& );
		bool operator() ( CComObjPtr<IMgaFCO>& );
		bool operator() ( CComObjPtr<IMgaFolder>& );
		bool operator() ( CComObjPtr<IMgaModel>& );
	private:
		CComObjPtr<IMgaObject>	m_objToFind;
		CComObjPtr<IMgaFCO>		m_fcoToFind;
		CComObjPtr<IMgaFolder>	m_folderToFind;
		CComObjPtr<IMgaModel>	m_modelToFind;
		int						m_which; // which variable is set
};

#define LIB_STUBS 0
#define LIB_STOP  1
#define LIB_CONT  2

class CMakeClosure
{
 public	: // operations
	CMakeClosure( 
		IMgaFCOs *fcos, IMgaFolders *fols,
		int ref = 1, int refd = 1, 
		int conn = 1, int refport_conn = 1,
		int set_mem = 1, int mem_of_set = 1, 
		int cont = 1, int part_of_mod = 1,
		int fold_cont = 1, int part_of_fold = 1,
		int base_type = 1, int derived_type = 1,
		int into_libr = LIB_STUBS, 
		int container_opt = 0,
		int direction_opt = 1,
		int all_kinds = -1,
		int mark_with = -1
		)
		: m_bRefersTo( ref == 1)
		, m_bReferredBy( refd == 1)
		, m_bConnection( conn == 1)
		, m_bRefPortConnection( refport_conn == 1)
		, m_bSetMember( set_mem == 1)
		, m_bMemberOfSets( mem_of_set == 1)
		, m_bContainment( cont == 1)
		, m_bPartOfModels( part_of_mod == 1)
		, m_bFolderContainment( fold_cont == 1)
		, m_bPartOfFolders( part_of_fold == 1)
		, m_bBaseTypes( base_type == 1)
		, m_bDerivedTypes( derived_type == 1)
		, m_iIntoLibraries( into_libr)
		, m_iContainerOption( container_opt)
		, m_bDirection( direction_opt == 1)
		, m_kindFilter( all_kinds)
		, m_markWith( mark_with)
	{
		if ( fcos)		CopyTo( fcos, m_selFcos);
		if ( fols)		CopyTo( fols, m_selFolds);

		if ( !m_selFcos.empty())		COMTHROW( m_selFcos.front()->get_Project( PutOut( m_project)) );
		else if ( !m_selFolds.empty())	COMTHROW( m_selFolds.front()->get_Project( PutOut( m_project)) );
		else							ASSERT( 0);

	}

	virtual ~CMakeClosure()
	{
		m_selFolds.clear();
		m_selFcos.clear();
		m_newlySelFolds.clear();
		m_newlySelFcos.clear();
		m_topFolds.clear();
		m_topFcos.clear();
		m_acceptingKindsAndFolders.clear();
	}
	
	long process();
	void getFCOs( IMgaFCOs **res_fcos);
	void getFolders( IMgaFolders **res_folds);
	
	bool noTopObjs(); // signals if The RootFolder is the common top (root) of all selected objects
	void clearTopObjs();
	void getTopFCOs( IMgaFCOs **res_fcos);
	void getTopFolders( IMgaFolders **res_folds);

	bool isAnyAccepting() const;
	void getAccKindsInString( std::string& p_resStr) const;
	const std::vector< std::string >& getAccKindsVector() const;

	void send2Console( const std::string& msg);

 protected:

	void initTrans(); // transaction
	void doneTrans( bool abort);
	static CComPtr<IGMEOLEApp> get_GME(CComObjPtr<IMgaProject> project);


	void doClosure();
	void processFolders();
	void processFCOs();

	virtual void preProcess();
	virtual void postProcess();
	void selectTop();
	void totalWrap();
	void autoWrap();
	std::string processUpward(IMgaObject *obj, CComObjPtrVector<IMgaFolder>& f_parents, CComObjPtrVector<IMgaModel>& m_parents);

	void checkMeta( const CComObjPtrVector<IMgaFolder>& p_folds, const CComObjPtrVector<IMgaFCO>& p_fcos);
	std::vector< std::string > m_acceptingKindsAndFolders; // the meta names of those kinds which the closure may be inserted into

	template<class T> void markObj( CComObjPtr<T> obj);
	virtual void procObj( IMgaObject* obj);
	
	void procFolder( IMgaFolder *folder);

	void procModel		( IMgaModel *model);
	void procAtom		( IMgaAtom *atom);
	void procReference	( IMgaReference *reference);
	void procSet		( IMgaSet *set);
	void procConnection	( IMgaConnection *connection);
	void procConnPoint	( IMgaConnPoint *connpoint);

	void procMyConnections		( IMgaFCO * fco);
	void procMyContainerSets	( IMgaFCO * fco);
	void procMyReferences		( IMgaFCO * fco);
	void procMyParent			( IMgaFCO * fco);
	void procMyParent			( IMgaFolder * fco);

	void procMyBaseTypes		( IMgaFCO * fco);
	void procMyDerivedTypes		( IMgaFCO * fco);

	bool isSimpleConnection( IMgaConnection * connection);
	virtual bool isValidSelection();
	virtual bool isValidFCO( IMgaFCO *fco);
	virtual bool isValidConnection( IMgaConnection * connection);
	bool isInLibrary( IMgaObject * obj);

	virtual bool postValidateSelection();
	virtual bool postValidateFCO( IMgaFCO *fco);
	virtual bool postValidateConnection( IMgaConnection * connection);

	bool findAmongSelected( const CComObjPtr<IMgaFCO>& in);
	bool findAmongSelected( const CComObjPtr<IMgaFolder>& in);
	bool findAmongNews( const CComObjPtr<IMgaFCO>& in);
	bool findAmongNews( const CComObjPtr<IMgaFolder>& in);

	void insertNew( const std::string& id, bool is_the_top = false);
	void insertNew( const CComObjPtr<IMgaFCO>& in);
	void insertNew( const CComObjPtrVector<IMgaFCO>& in_v);
	void insertNew( const CComObjPtr<IMgaFolder>& in);
	void insertNew( const CComObjPtrVector<IMgaFolder>& in_v);
	void insertNew( const CComObjPtr<IMgaModel>& in);

	template<class INTERFACE, class FUNC_INTERFACE, class COLLECTION>
	void insertNew(INTERFACE p, HRESULT (__stdcall FUNC_INTERFACE::*func)(COLLECTION **q))
	{
		FUNC_INTERFACE *q = p;
		ASSERT( q != NULL );

		typedef TYPENAME_COLL2ELEM(COLLECTION) element_type;

		CComObjPtrVector<element_type> v;
		COMTHROW( (q->*func)(PutOut(v)) );

		CComObjPtrVector<element_type>::iterator i = v.begin();
		CComObjPtrVector<element_type>::iterator e = v.end();
		while( i != e )
		{
			if( m_kindFilter == -1)
				insertNew( *i);
			else
			{
				CComObjPtr<element_type> elem( *i);
				CComObjPtr<IMgaFolder> f;
				CComObjPtr<IMgaModel> m;
				CComObjPtr<IMgaAtom> a;
				CComObjPtr<IMgaSet> s;
				CComObjPtr<IMgaReference> r;
				::QueryInterface( elem, f);
				::QueryInterface( elem, m);
				::QueryInterface( elem, a);
				::QueryInterface( elem, s);
				::QueryInterface( elem, r);

				if(    f && ( m_kindFilter & 0x1)
					|| m && ( m_kindFilter & 0x2)
					|| a && ( m_kindFilter & 0x4)
					|| s && ( m_kindFilter & 0x8)
					|| r && ( m_kindFilter & 0x10))
					insertNew( *i);
			}

			++i;
		}
	}

 protected: // variables
	CComPtr<IGMEOLEApp> m_GME; // to access the console
		
	CComObjPtr<IMgaProject> m_project;
	CComObjPtr<IMgaTerritory> m_territory;

	CComObjPtrVector<IMgaFolder> m_selFolds; // folders already in closure
	CComObjPtrVector<IMgaFCO> m_selFcos; // fcos already in closure

	CComObjPtrVector<IMgaFCO> m_newlySelFcos; // will be copied into selfcos
	CComObjPtrVector<IMgaFolder> m_newlySelFolds; // will be copied into selfolders

	// members used for the non-miniproject cases
	CComObjPtrVector<IMgaFolder> m_topFolds; // parentless (top) folders
	CComObjPtrVector<IMgaFCO> m_topFcos; // parentless (top) fcos


	bool m_bRefersTo;	// objects references refer to
	bool m_bReferredBy; // references to an fco

	bool m_bConnection; // fcos on the other end of a connection and the connections themselves
	bool m_bRefPortConnection; // special connections: thru reference ports

	bool m_bSetMember;		// members of a set
	bool m_bMemberOfSets;	// all sets containing the fco

	bool m_bContainment;	// model containment, downward
	bool m_bPartOfModels;	// model containment, upward

	bool m_bFolderContainment;	// folder containment, downward
	bool m_bPartOfFolders;		// folder containment, upward

	bool m_bBaseTypes;		// base types of selected objects
	bool m_bDerivedTypes;	// derived types of selected objects

	int m_iIntoLibraries;	// 0 stubs, 1 stop, 2 continue process through links that point into a library?

	int m_iContainerOption;	// wrap? 0: miniproj, 1: auto, 2: asis, 

	bool m_bDirection;		// up or down?

	int m_kindFilter;		// if -1 then all kinds, otherwise bitmasking needed to calc which kinds are considered from the children
	int m_markWith;         // if -1 then don't mark, otherwise mark all objects in closure (into registry)
	static const wchar_t* m_markerNode;
};

#endif // MAKE_CLOSURE_H
