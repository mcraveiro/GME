#include "stdafx.h"
#include "CommonSmart.h"
#include "GMEGraph.h"
#include "Gme.h"

GMEGraph::GMEGraph( IMgaProject *project, IMgaModel* model, IMgaMetaAspect* aspect )
{
    CComObjPtr<IMgaParts> parts;

    COMTHROW( model->get_AspectParts(aspect, 0, PutOut(parts)) );

    fillNodes( project, parts );
    fillConnections( project, parts );

    fillSubGraphField();
}

GMEGraph::~GMEGraph()
{
    for( unsigned int i=0; i<m_nodes.size(); ++i )
    {
        if( m_nodes[i]->m_fco != NULL )
        {
            m_nodes[i]->m_fco->Release();
            m_nodes[i]->m_fco = NULL;
        }
        for( unsigned int j=0; j<m_nodes[i]->m_ports.size(); ++j )
        {
            if( m_nodes[i]->m_ports[j]->m_fco != NULL )
            {
                m_nodes[i]->m_ports[j]->m_fco->Release();
                m_nodes[i]->m_ports[j]->m_fco = NULL;
            }
        }
    }

    clear();
}


void GMEGraph::fillNodes( IMgaProject *project, IMgaParts* parts )
{
    long   n;

    COMTHROW( parts->get_Count(&n) );
    for( int i=0; i<n; ++i )
    {
        CComObjPtr<IMgaPart>     part;
        CComObjPtr<IMgaFCO>      fco;
        CComObjPtr<IMgaMetaPart> metaPart;
        objtype_enum             type;
        CComBSTR                 icon;
        long                     x,y,sx,sy;

        COMTHROW( parts->get_Item(i+1, PutOut(part)) );
        COMTHROW( part->get_FCO(PutOut(fco)) );
        COMTHROW( part->get_Meta(PutOut(metaPart)) );
        COMTHROW( fco->get_ObjType(&type) );
        COMTHROW( part->GetGmeAttrs(&icon,&x,&y) );

		// FIXME what about SET
        if( type == OBJTYPE_ATOM || type == OBJTYPE_MODEL || type == OBJTYPE_REFERENCE )
        {
            CComBSTR decoratorProgID;
            COMTHROW( fco->get_RegistryValue(L"decorator",&decoratorProgID) );

            CComObjPtr<IMgaDecorator> decorator;
			CComPtr<IMgaElementDecorator> newDecorator;
            if (decoratorProgID.Length() == 0)
                decoratorProgID = L"Mga.BoxDecorator";

			try {
				HRESULT hres = newDecorator.CoCreateInstance(PutInBstr(decoratorProgID));
				if (FAILED(hres) && hres != CO_E_CLASSSTRING) {	// might be an old decorator
					hres = decorator.CoCreateInstance(PutInBstr(decoratorProgID));
				}
				if (hres == S_OK && newDecorator)
					decorator = CComQIPtr<IMgaDecorator>(newDecorator);
				if (hres == S_OK && decorator) {
					if (newDecorator)
						COMTHROW(newDecorator->InitializeEx(project, metaPart, fco, NULL, (ULONGLONG)GetDesktopWindow()));
					else
						COMTHROW(decorator->Initialize(project, metaPart, fco));

					COMTHROW( decorator->GetPreferredSize( &sx, &sy ) );
					// round sx and sy up to nearest GME_GRID_SIZE multiple
					sx = (sx + GME_GRID_SIZE - 1) / GME_GRID_SIZE * GME_GRID_SIZE;
					sy = (sy + GME_GRID_SIZE - 1) / GME_GRID_SIZE * GME_GRID_SIZE;
					COMTHROW( decorator->SetLocation( x, y, x+sx, y+sy ) );

					Node * node = new Node( sx, sy );
					node->m_x = x;
					node->m_y = y;
					node->m_fco = fco;
					node->m_fco->AddRef();

					CComObjPtr<IMgaFCOs> fcos;
					COMTHROW( decorator->GetPorts(PutOut(fcos)) );
					long fcoNum = 0;
					if( fcos!=NULL )
						COMTHROW( fcos->get_Count(&fcoNum) );
					for( int j=0; j<fcoNum; ++j )
					{
						CComObjPtr<IMgaFCO> port_fco;
						COMTHROW( fcos->get_Item(j+1, PutOut(port_fco)) );

						long port_sx, port_sy, port_ex, port_ey;
						COMTHROW( decorator->GetPortLocation( port_fco, &port_sx, &port_sy, &port_ex, &port_ey ) );

						int x1 = port_ex;
						int x2 = port_sx;
						if( x2 < x1 )
						{
							x1 = port_sx;
							x2 = port_ex;
						}
						int y1 = port_ey;
						int y2 = port_sy;
						if( y2 < y1 )
						{
							y1 = port_sy;
							y2 = port_ey;
						}

						Node * port = new Node( node, x1, y1, x2-x1, y2-y1 );
						port->m_fco = port_fco;
						port->m_fco->AddRef();
						node->m_ports.push_back( port );
					}
					m_nodes.push_back( node );
				}
			}
			catch (hresult_exception&) {
			}
			if (decorator) {
				decorator->Destroy();
			}
        }
    }
}

void GMEGraph::fillConnections( IMgaProject *project, IMgaParts* parts )
{
    long   n;

    COMTHROW( parts->get_Count(&n) );
    for( int i=0; i<n; ++i )
    {
        CComObjPtr<IMgaPart>     part;
        CComObjPtr<IMgaFCO>      fco;
        objtype_enum             type;
        
        COMTHROW( parts->get_Item(i+1, PutOut(part)) );
        COMTHROW( part->get_FCO(PutOut(fco)) );
        COMTHROW( fco->get_ObjType(&type) );

        if( type == OBJTYPE_CONNECTION )
        {
            CComObjPtr<IMgaSimpleConnection> conn( (IMgaSimpleConnection*)fco.p );

            CComObjPtr<IMgaFCO> fco_from;
            CComObjPtr<IMgaFCO> fco_to;

            COMTHROW( conn->get_Src(PutOut(fco_from)) );
            COMTHROW( conn->get_Dst(PutOut(fco_to)) );

            Node * nodeFrom = findFCO( fco_from.p );
            Node * nodeTo   = findFCO( fco_to.p );

            if( nodeFrom != NULL && nodeTo != NULL )
            {
                Edge * e = new Edge( nodeFrom, nodeTo );

                CComBSTR prefs;
                COMTHROW( fco->get_RegistryValue( L"autorouterPref", &prefs ) );
                CString prefs2(prefs);
                setRoutingPrefs(e,prefs2);

                m_edges.push_back( e );
                nodeFrom->m_edges.push_back( e );
                if( nodeFrom->m_parent != NULL )
                    nodeFrom->m_parent->m_edges.push_back(e);
                if( nodeFrom != nodeTo )
                {
                    nodeTo->m_edges.push_back( e );
                    if( nodeTo->m_parent != NULL )
                        nodeTo->m_parent->m_edges.push_back(e);
                }
            }
        }
    }
}

Node * GMEGraph::findFCO( IMgaFCO * fco )
{
    for( unsigned int i=0; i<m_nodes.size(); ++i )
    {
        Node * n = m_nodes[i];
        if( n->m_fco == fco )
            return n;
        for( unsigned int j=0; j<n->m_ports.size(); ++j )
        {
            Node * p = n->m_ports[j];
            if( p->m_fco == fco )
                return p;
        }
    }
    return NULL;
}

void GMEGraph::setRoutingPrefs( Edge * e, CString connPrefs )
{
    bool nodeFromIsPort = (e->m_nodeFrom->m_parent != NULL);
    bool nodeToIsPort   = (e->m_nodeTo->m_parent != NULL);

    if( nodeFromIsPort )
    {
        if( e->m_nodeFrom->m_x < e->m_nodeFrom->m_parent->m_sx/2 )
            e->cannotStartToEast = true;
        else
            e->cannotStartToWest = true;
    }
    else
    {
        CComBSTR prefs;
        COMTHROW( e->m_nodeFrom->m_fco->get_RegistryValue( CComBSTR(L"autorouterPref"), &prefs ) );
        CString prefs2(prefs);
        if( connPrefs.GetLength() > 0 )
            prefs2 = connPrefs;
        if( prefs2.GetLength() > 0 )
        {
            bool N = prefs2.Find(_T("N"))!=-1;
            bool E = prefs2.Find(_T("E"))!=-1;
            bool S = prefs2.Find(_T("S"))!=-1;
            bool W = prefs2.Find(_T("W"))!=-1;

            if( !N && !E &&  S && !W ) e->cannotStartToNorth = true;
            if( !N && !E && !S &&  W ) e->cannotStartToEast  = true;
            if(  N && !E && !S && !W ) e->cannotStartToSouth = true;
            if( !N &&  E && !S && !W ) e->cannotStartToWest  = true;
        }
    }

    if( nodeToIsPort )
    {
        if( e->m_nodeTo->m_x < e->m_nodeTo->m_parent->m_sx/2 )
            e->cannotEndFromEast = true;
        else
            e->cannotEndFromWest = true;
    }
    else
    {
        CComBSTR prefs;
        COMTHROW( e->m_nodeTo->m_fco->get_RegistryValue( CComBSTR(L"autorouterPref"), &prefs ) );
        CString prefs2(prefs);
        if( connPrefs.GetLength() > 0 )
            prefs2 = connPrefs;
        if( prefs2.GetLength() > 0 )
        {
            bool N = prefs2.Find(_T("n"))!=-1;
            bool E = prefs2.Find(_T("e"))!=-1;
            bool S = prefs2.Find(_T("s"))!=-1;
            bool W = prefs2.Find(_T("w"))!=-1;

            if( !N && !E &&  S && !W ) e->cannotEndFromNorth = true;
            if( !N && !E && !S &&  W ) e->cannotEndFromEast  = true;
            if(  N && !E && !S && !W ) e->cannotEndFromSouth = true;
            if( !N &&  E && !S && !W ) e->cannotEndFromWest  = true;
        }
    }
}