#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>

#define GME_GRID_SIZE 7 // from GME GMEStd.h

struct Node;
struct Edge;

typedef std::vector<Node*> NodeVec;
typedef std::vector<Edge*> EdgeVec;

struct IMgaFCO;

struct Node
{
    Node( int sx, int sy )
    {
        m_sx     = sx;
        m_sy     = sy;
        m_parent = NULL;
        m_fco    = NULL;
    }

    Node( Node * parent, int x, int y, int sx, int sy )
    {
        m_parent = parent;
        m_x      = x;
        m_y      = y;
        m_sx     = sx;
        m_sy     = sy;
        m_fco    = NULL;
    }

    int        m_nodeId;
    bool       m_connectedToOthers;
    int        m_subGraph;     // for graph coloring algorithms
    Node *     m_parent;
    int        m_x;
    int        m_y;
    int        m_sx;
    int        m_sy;
    NodeVec    m_ports;
    EdgeVec    m_edges;
    IMgaFCO *  m_fco;
};

struct Edge
{
    Edge( Node * nodeFrom, Node * nodeTo )
    {
        m_nodeFrom         = nodeFrom;
        m_nodeTo           = nodeTo;
        cannotStartToEast  = false;
        cannotStartToEast  = false;
        cannotStartToWest  = false;
        cannotStartToNorth = false;
        cannotStartToSouth = false;
        cannotEndFromEast  = false;
        cannotEndFromWest  = false;
        cannotEndFromNorth = false;
        cannotEndFromSouth = false;
    }

    Node * getTopLevelFrom()
    {
        if( m_nodeFrom == NULL || m_nodeFrom->m_parent == NULL )
            return m_nodeFrom;
        else
            return m_nodeFrom->m_parent;
    }

    Node * getTopLevelTo()
    {
        if( m_nodeTo == NULL || m_nodeTo->m_parent == NULL )
            return m_nodeTo;
        else
            return m_nodeTo->m_parent;
    }

    Node * m_nodeFrom;
    Node * m_nodeTo;
    bool   cannotStartToEast;
    bool   cannotStartToWest;
    bool   cannotStartToNorth;
    bool   cannotStartToSouth;
    bool   cannotEndFromEast;
    bool   cannotEndFromWest;
    bool   cannotEndFromNorth;
    bool   cannotEndFromSouth;
    int    x1;
    int    y1;
    int    x2;
    int    y2;
};

class Graph
{
public:
                Graph                      ();

                Graph                      ( int nodeNum, int edgeNum );

    virtual    ~Graph                      ();

    int         getNumberOfSubGraphs       ();

//protected:
    void        fillConnectedToOthersFiled ();

    void        fillSubGraphFieldForOneNode( Node * n, int subGraph );

    void        fillSubGraphField          ();

    void        clear();

    int         m_numberOfSubGraphs;
    NodeVec     m_nodes;
    EdgeVec     m_edges;

};

#endif // _GRAPH_H_
