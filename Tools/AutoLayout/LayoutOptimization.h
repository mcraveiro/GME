#ifndef _LAYOUTOPTIMIZATION_H_
#define _LAYOUTOPTIMIZATION_H_

#pragma warning( disable : 4786) 

#include <set>
#include "GAOptimizer.h"
#include "Graph.h"

#define XMARGIN     (GME_GRID_SIZE * 5)
#define YMARGIN     (GME_GRID_SIZE * 3)

typedef std::vector<int>  IntVec;
typedef std::set<int>     IntSet;

class LayoutSolution;

class LayoutOptimizerListener
{
public:
	enum ContinueAbortOrCurrent {
		CONTINUE,
		ABORT,
		CURRENT,
	};
    virtual ContinueAbortOrCurrent update( int percentage, LayoutSolution * sol, double score ) = 0;
};

class LayoutOptimizer
{
public:
                        LayoutOptimizer     ( Graph * graph );

    void                optimize            ( LayoutOptimizerListener * listener = NULL, bool startFromScratch=true );

private:
    Graph * m_graph;
};

class LayoutOptProblem: public GAOptimizer::IProblem
{
public:
                                     LayoutOptProblem     ( Graph * graph, int subGraph, bool startFromScratch );



    virtual GAOptimizer::IGenotype * createRandomSolution ( int i );
    
    virtual double                   evaluteSolution      ( GAOptimizer::IGenotype * solution );

    int                              getWidth             ()  { return m_width; }

    int                              getHeight            ()  { return m_height; }

    EdgeVec&                         getEdges             ()  { return m_edges; }

private:
    bool        m_startFromScratch;
    NodeVec     m_nodes;
    EdgeVec     m_edges;
    int         m_nodeNum;
    int         m_width;
    int         m_height;

    friend class LayoutSolution;
    friend class LayoutOptimizer;
};

struct NodePos
{
    Node * m_node;
    int    m_x;
    int    m_y;
    int    m_tempx;
    int    m_tempy;
};

typedef std::vector<NodePos> NodePosVec;

class LayoutSolution: public GAOptimizer::IGenotype
{
public:
                LayoutSolution           ( LayoutOptProblem * problem );

    void        derive                   ( GAOptimizer::IGenotype * parent1, 
                                           GAOptimizer::IGenotype * parent2 );

    void        random                   ();

    void        copyFromGraph            ();

    bool        areConnectionsCrossed    ( Edge * e1, Edge * e2 );

    static bool areLinesCrossed          ( int x1, int y1, int x2, int y2, int xp1, int yp1, int xp2, int yp2 );
    
    int         getDirViolations         ( Edge * e );

    double      getScore                 ();

    NodePosVec& getNodes                 () { return m_nodes; }

    LayoutOptProblem* getProblem         () { return m_problem; }

    void        calcBoundingBox          ();

    void        crop                     ();

    void        move                     ( int dx, int dy );

    void        calcEdgeEnds             ( Edge * e, int& x1, int& y1, int& x2, int& y2 );

private:
    void    calcEdgeEnds         ( Edge * e );

    bool    isNodePlaceable      ( int node, int x, int y );

    void    placeNode            ( int node, int x, int y );

    void    removeNode           ( int node );

    void    placeNodeNearPos     ( int node, int xorg, int yorg );

    void    placeNodeToRandomPos ( int node );

    void    randomSwap           ();

    void    moveOneToRandomPos   ();

    void    selectRandomNodes    ( IntSet& nodes );

    void    addSubGraphNodes     ( Node * act, IntSet& nodes, Node * prohibited, double cutoff );

    void    selectSubGraph       ( IntSet& nodes );

    void    moveSome             ();

    void    moveOne              ();

    void    mutate               ();
    

    LayoutOptProblem  *  m_problem;
    NodePosVec           m_nodes;

    int                  m_xmin;
    int                  m_ymin;
    int                  m_xmax;
    int                  m_ymax;

    friend class LayoutOptimizer;
};



#endif // _LAYOUTOPTIMIZATION_H_
