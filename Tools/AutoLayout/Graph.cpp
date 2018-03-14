#include "stdafx.h"
#include "Random.h"
#include "Graph.h"

Graph::Graph()
{
}

Graph::Graph( int nodeNum, int edgeNum )
{
    int i,j;

    // create nodes
    for( i=0; i<nodeNum; ++i )
    {
        Node * n = new Node(60, 40);
        m_nodes.push_back( n );
    }
                    
    // create random edges
    while( true )
    {        
        Node * nodeFrom = m_nodes[Random::nextInt(nodeNum)];
        Node * nodeTo   = m_nodes[Random::nextInt(nodeNum)];
        
        // check if we already have that edge
        for( j=0; j<m_edges.size(); ++j )
        {
            Edge * e2 = m_edges[j];
            if(e2->m_nodeFrom == nodeFrom && e2->m_nodeTo == nodeTo)
               break;                
        }
        if( j==m_edges.size() )
        {
            Edge * e = new Edge( nodeFrom, nodeTo );
            m_edges.push_back( e );            
            nodeFrom->m_edges.push_back( e );
            if( nodeFrom != nodeTo )
                nodeTo->m_edges.push_back( e );
            if( m_edges.size() == edgeNum )
                break;
        }
    }

    fillSubGraphField();
}

Graph::~Graph()
{
    clear();
}

int Graph::getNumberOfSubGraphs()
{
    return m_numberOfSubGraphs;
}

void Graph::fillConnectedToOthersFiled()
{
    for( int i=0; i<m_nodes.size(); ++i )
    {        
        Node * n = m_nodes[i];
        n->m_connectedToOthers = false;
        for( int j=0; j<n->m_edges.size(); ++j )
        {
            if( n->m_edges[j]->m_nodeFrom != n || n->m_edges[j]->m_nodeTo != n )
            {
                n->m_connectedToOthers = true;
                break;
            }
        }
    }
}

void Graph::fillSubGraphFieldForOneNode( Node * n, int subGraph )
{
    n->m_subGraph = subGraph;
    for( int i=0; i<n->m_edges.size(); ++i )
    {
        Node * n2 = n->m_edges[i]->getTopLevelFrom();
        if( n2 == n )
            n2 = n->m_edges[i]->getTopLevelTo();
        if( n2 != n && n2->m_subGraph == -1 )
            fillSubGraphFieldForOneNode( n2, subGraph );
    }
}

void Graph::fillSubGraphField()
{
    int i;

    fillConnectedToOthersFiled();

    for( i=0; i<m_nodes.size(); ++i )
    {
        m_nodes[i]->m_subGraph = -1;
        m_nodes[i]->m_nodeId = i;
    }

    m_numberOfSubGraphs = 0;
    for( i=0; i<m_nodes.size(); ++i )
    {
        if( m_nodes[i]->m_subGraph == -1 && m_nodes[i]->m_connectedToOthers )
        {
            fillSubGraphFieldForOneNode(m_nodes[i],m_numberOfSubGraphs);
            m_numberOfSubGraphs++;
        }
    }
}

void Graph::clear()
{
    int i;

    for( i=0; i<m_nodes.size(); ++i )
        delete m_nodes[i];
    for( i=0; i<m_edges.size(); ++i )
        delete m_edges[i];
    
    m_nodes.clear();
    m_edges.clear();
}
