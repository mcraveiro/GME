#include "stdafx.h"
#include <cmath>
#include <cfloat>
#include "Random.h"
#include "LayoutOptimization.h"
#include "GAOptimizer.h"

#define MAX_OPTIMIZATION_TRIES 50
#define FITNESS_RELATIVE_ERROR_THRESHOLD 0.002
#define FITNESS_AVERAGE_START -2e5

using GAOptimizer::IGenotype;
using GAOptimizer::IProblem;

#ifdef max
#undef max
#endif

LayoutOptimizer::LayoutOptimizer( Graph * graph )
{
    m_graph = graph;
}

void LayoutOptimizer::optimize( LayoutOptimizerListener * listener, bool startFromScratch )
{
    int i,j;
    int x = 0;
    int m = MAX_OPTIMIZATION_TRIES;
	int maxy = 0;
	LayoutOptimizerListener::ContinueAbortOrCurrent status = LayoutOptimizerListener::ContinueAbortOrCurrent::CONTINUE;

    for( i=0; i<m_graph->getNumberOfSubGraphs() && status == LayoutOptimizerListener::CONTINUE; ++i )
    {
        LayoutOptProblem       problem( m_graph, i, startFromScratch );
        GAOptimizer::Optimizer optimizer;

        optimizer.init( &problem, 500, 20 );

        double fitnessAverage = FITNESS_AVERAGE_START;
        LayoutSolution * best;
		for( j=0; j<m && status == LayoutOptimizerListener::CONTINUE; ++j )
        {
            optimizer.step(800);
            best = (LayoutSolution*)optimizer.getBest();
            double maxFitness = optimizer.getMaxFitness();

            if (listener != NULL)
                status = listener->update( (int)(100 * (i*m+j) / double(m_graph->getNumberOfSubGraphs() * m)), best, maxFitness );
            fitnessAverage = fitnessAverage / 2 + maxFitness / 2;
            if (fabs((fitnessAverage - maxFitness) / maxFitness) < FITNESS_RELATIVE_ERROR_THRESHOLD)
                break;
        }

		if ( status != LayoutOptimizerListener::ABORT )
		{
			// get best, place it, write back positions to m_graph
			best->crop();
			best->move( x, 0 );
			x += (best->m_xmax - best->m_xmin);
			for( j=0; j<best->m_nodes.size(); ++j )
			{
				best->m_nodes[j].m_node->m_x = best->m_nodes[j].m_x;
				best->m_nodes[j].m_node->m_y = best->m_nodes[j].m_y;
				maxy = std::max(maxy, best->m_nodes[j].m_y + m_graph->m_nodes[j]->m_sy);
			}
		}
    }

	if ( status != LayoutOptimizerListener::ABORT )
	{
		// place not connected nodes in rows
		int y = YMARGIN + maxy;
		int max_y_size = 0;
		x = XMARGIN;
		for( i=0; i<m_graph->m_nodes.size(); ++i )
		{
			if( !(m_graph->m_nodes[i]->m_connectedToOthers) )
			{
				if ((m_graph->m_nodes[i]->m_sx + XMARGIN + x > 600)
					&& (x > 100))
				{
					x = XMARGIN;
					y += max_y_size + YMARGIN;
					max_y_size = 0;
				}
				m_graph->m_nodes[i]->m_x = x;
				m_graph->m_nodes[i]->m_y = y;
				x += m_graph->m_nodes[i]->m_sx + XMARGIN;
				max_y_size = std::max(max_y_size, m_graph->m_nodes[i]->m_sy);
			}
		}
	}
}

LayoutOptProblem::LayoutOptProblem( Graph * graph, int subGraph, bool startFromScratch )
{
    int i,j,k;

    m_nodeNum  = 0;

    m_startFromScratch = startFromScratch;

    // calcualte area size, set nodeids in graph, fill m_nodes, m_edges
    double area = 0;
    for( i=0; i<graph->m_nodes.size(); ++i )
    {
        Node * n = graph->m_nodes[i];
        if( n->m_subGraph == subGraph )
        {
            area += ((2*XMARGIN+n->m_sx) * (2*YMARGIN+n->m_sy));

            n->m_nodeId = m_nodeNum;
            //n->m_x      = -1;
            //n->m_y      = -1;

            m_nodes.push_back( n );

            // add edges
            for( j=0; j<n->m_edges.size(); ++j )
            {
                Edge * edgeToAdd = n->m_edges[j];

                // find if the edge already added
                for( k=0; k<m_edges.size(); ++k )
                    if( edgeToAdd == m_edges[k] )
                        break;
                if( k==m_edges.size())
                    m_edges.push_back( edgeToAdd );
            }

            m_nodeNum++;
        }
    }
    m_width  = (int)(3 * sqrt( area ));
    m_height = m_width;
}


IGenotype * LayoutOptProblem::createRandomSolution(int i)
{
    LayoutSolution * sol = new LayoutSolution(this);
    if( i<2 && !m_startFromScratch)
        sol->copyFromGraph();
    return sol;
}
    
double LayoutOptProblem::evaluteSolution( IGenotype * solution )
{
    LayoutSolution * sol = (LayoutSolution*)solution;
    return sol->getScore();
}

LayoutSolution::LayoutSolution( LayoutOptProblem * problem )
{
    m_problem = problem;

    m_nodes.resize( problem->m_nodeNum );
    for( int i=0; i<problem->m_nodeNum; ++i )
        m_nodes[i].m_node = problem->m_nodes[i];

    random();
}

void LayoutSolution::derive( GAOptimizer::IGenotype * parent1, GAOptimizer::IGenotype * parent2 )
{
    LayoutSolution* p1 = (LayoutSolution*)parent1;
    LayoutSolution* p2 = (LayoutSolution*)parent2;
    
    // crossing over
    int n = m_nodes.size();
    if( Random::nextDouble() < 0.8 )
    {
        for( int i=0; i<n; ++i )
        {            
            if( Random::nextDouble() < 0.5 )
                placeNodeNearPos(i, p1->m_nodes[i].m_x, p1->m_nodes[i].m_y );
            else 
                placeNodeNearPos(i, p2->m_nodes[i].m_x, p2->m_nodes[i].m_y );
        }        
    }
    else
    {
        for( int i=0; i<n; ++i )
            placeNodeNearPos(i, p1->m_nodes[i].m_x, p1->m_nodes[i].m_y );
    }
    
    // mutation
    mutate();
}

void LayoutSolution::random()
{
    for( int i=0; i<m_nodes.size(); ++i )
        placeNodeToRandomPos(i);
}

void LayoutSolution::copyFromGraph()
{
    for( int i=0; i<m_nodes.size(); ++i )
    {
        int x = m_nodes[i].m_node->m_x;
        int y = m_nodes[i].m_node->m_y;
        placeNodeNearPos(i, x, y );
    }
}


bool LayoutSolution::areConnectionsCrossed( Edge * e1, Edge * e2 )
{
    if( e1 == e2 )
        return false;

    calcEdgeEnds( e1 );
    calcEdgeEnds( e2 );

    return areLinesCrossed( e1->x1, e1->y1, e1->x2, e1->y2, e2->x1, e2->y1, e2->x2, e2->y2 );
}

bool LayoutSolution::areLinesCrossed( int x1, int y1, int x2, int y2, int xp1, int yp1, int xp2, int yp2 )
{
    if( (x1==xp1 && y1==yp1) || (x1==xp2 && y1==yp2) || (x2==xp1 && y2==yp1) || (x2==xp2 && y2==yp2) )
        return false;

    int diffX  = x2-x1; 
    int diffY  = y2-y1; 
    int diffXp = xp2-xp1; 
    int diffYp = yp2-yp1; 

    return((((diffX*yp1-diffY*xp1)<(diffX*y1-diffY*x1))^((diffX*yp2-diffY*xp2)<(diffX*y1-diffY*x1))) & 
       (((diffXp*y1-diffYp*x1)<(diffXp*yp1-diffYp*xp1))^((diffXp*y2-diffYp*x2)<(diffXp*yp1-diffYp*xp1))));
}

int LayoutSolution::getDirViolations( Edge * e )
{
    int violations = 0;

    if( e->cannotStartToNorth && e->y2<e->y1+2*YMARGIN )
        violations++;
    else if( e->cannotEndFromNorth && e->y1<e->y2+2*YMARGIN )
        violations++;

    if( e->cannotStartToEast && e->x2>e->x1-2*XMARGIN )
        violations++;
    else if( e->cannotEndFromEast && e->x1>e->x2-2*XMARGIN )
        violations++;

    if( e->cannotStartToSouth && e->y2>e->y1-2*YMARGIN )
        violations++;
    else if( e->cannotEndFromSouth && e->y1>e->y2-2*YMARGIN )
        violations++;

    if( e->cannotStartToWest && e->x2<e->x1+2*XMARGIN )
        violations++;
    else if( e->cannotEndFromWest && e->x1<e->x2+2*XMARGIN )
        violations++;

    /*if( e->cannotStartToNorth && e->y2<e->y1+2*YMARGIN )
        violations++;
    if( e->cannotStartToEast && e->x2>e->x1-2*XMARGIN )
        violations++;
    if( e->cannotStartToSouth && e->y2>e->y1-2*YMARGIN )
        violations++;
    if( e->cannotStartToWest && e->x2<e->x1+2*XMARGIN )
        violations++;
    if( e->cannotEndFromNorth && e->y1<e->y2+2*YMARGIN )
        violations++;
    if( e->cannotEndFromEast && e->x1>e->x2-2*XMARGIN )
        violations++;
    if( e->cannotEndFromSouth && e->y1>e->y2-2*YMARGIN )
        violations++;
    if( e->cannotEndFromWest && e->x1<e->x2+2*XMARGIN )
        violations++;*/

    return violations;
}

void LayoutSolution::calcBoundingBox()
{
    m_xmin = INT_MAX;
    m_ymin = INT_MAX;
    m_xmax = -INT_MAX;
    m_ymax = -INT_MAX;

    int n = m_nodes.size();
    for( int i=0; i<n; ++i )
    {
        if( m_nodes[i].m_x-XMARGIN < m_xmin )
            m_xmin = m_nodes[i].m_x-XMARGIN;
        if( m_nodes[i].m_y-YMARGIN < m_ymin )
            m_ymin = m_nodes[i].m_y-YMARGIN;
        if( m_nodes[i].m_x+m_nodes[i].m_node->m_sx + XMARGIN > m_xmax )
            m_xmax = m_nodes[i].m_x+m_nodes[i].m_node->m_sx + XMARGIN;
        if( m_nodes[i].m_y+m_nodes[i].m_node->m_sy + YMARGIN > m_ymax )
            m_ymax = m_nodes[i].m_y+m_nodes[i].m_node->m_sy + YMARGIN;
    }
}

void LayoutSolution::crop()
{
    calcBoundingBox();
    move( -m_xmin, -m_ymin );    
}

void LayoutSolution::move( int dx, int dy )
{
    calcBoundingBox();
    int n = m_nodes.size();
    for( int i=0; i<n; ++i )
    {
        m_nodes[i].m_x += dx;
        m_nodes[i].m_y += dy;
    }
    m_xmin += dx;
    m_ymin += dy;
    m_xmax += dx;
    m_ymax += dy;
}

void LayoutSolution::calcEdgeEnds( Edge * e, int& x1, int& y1, int& x2, int& y2 )
{
    if( e->m_nodeFrom->m_parent == NULL )
    {
        x1 = m_nodes[e->m_nodeFrom->m_nodeId].m_x + e->m_nodeFrom->m_sx / 2;
        y1 = m_nodes[e->m_nodeFrom->m_nodeId].m_y + e->m_nodeFrom->m_sy / 2;
    }
    else
    {
        x1 = m_nodes[e->m_nodeFrom->m_parent->m_nodeId].m_x + e->m_nodeFrom->m_x + e->m_nodeFrom->m_sx / 2;
        y1 = m_nodes[e->m_nodeFrom->m_parent->m_nodeId].m_y + e->m_nodeFrom->m_y + e->m_nodeFrom->m_sy / 2;
    }
    if( e->m_nodeTo->m_parent == NULL )
    {
        x2 = m_nodes[e->m_nodeTo->m_nodeId].m_x + e->m_nodeTo->m_sx / 2;
        y2 = m_nodes[e->m_nodeTo->m_nodeId].m_y + e->m_nodeTo->m_sy / 2;
    }
    else
    {
        x2 = m_nodes[e->m_nodeTo->m_parent->m_nodeId].m_x + e->m_nodeTo->m_x + e->m_nodeTo->m_sx / 2;
        y2 = m_nodes[e->m_nodeTo->m_parent->m_nodeId].m_y + e->m_nodeTo->m_y + e->m_nodeTo->m_sy / 2;
    }
}

void LayoutSolution::calcEdgeEnds( Edge * e )
{
    calcEdgeEnds( e, e->x1, e->y1, e->x2, e->y2 );
}

bool LayoutSolution::isNodePlaceable( int node, int x, int y )
{
    NodePos& n = m_nodes[node];

    // check if out of search region
    if( x<XMARGIN || y<YMARGIN || x+n.m_node->m_sx+XMARGIN>=m_problem->m_width     
        || y+n.m_node->m_sy+YMARGIN>=m_problem->m_height )
        return false;

    // check if intersets with others
    int x1  = x - XMARGIN;
    int y1  = y - YMARGIN;
    int sx1 = n.m_node->m_sx + 2 * XMARGIN;
    int sy1 = n.m_node->m_sy + 2 * YMARGIN;
    for( int i=0; i<m_nodes.size(); ++i )
    {
        if( i!=node)
        {
            NodePos& n2 = m_nodes[i];

            int x2  = n2.m_x - XMARGIN;
            int y2  = n2.m_y - YMARGIN;
            int sx2 = n2.m_node->m_sx + 2 * XMARGIN;
            int sy2 = n2.m_node->m_sy + 2 * YMARGIN;

            if( !((x2 + sx2 <= x1) ||
                  (y2 + sy2 <= y1) ||
                  (x2 >= x1 + sx1) ||
                  (y2 >= y1 + sy1)))
            {
                return false;
            }
        }
    }
    return true;
}

void LayoutSolution::placeNode( int node, int x, int y )
{
    m_nodes[node].m_x = x;
    m_nodes[node].m_y = y;
}

void LayoutSolution::removeNode( int node )
{
    m_nodes[node].m_x = -1;
    m_nodes[node].m_y = -1;
}

void LayoutSolution::placeNodeNearPos( int node, int xorg, int yorg )
{
    int maxx = m_problem->m_width  - m_nodes[node].m_node->m_sx - XMARGIN;
    int maxy = m_problem->m_height - m_nodes[node].m_node->m_sy - YMARGIN;
    
    if( xorg<XMARGIN)
        xorg = XMARGIN;
    if( xorg>=maxx )
        xorg = maxx;
    if( yorg<YMARGIN)
        yorg = YMARGIN;
    if( yorg>=maxy )
        yorg = maxy;
    
    int    x    = xorg;
    int    y    = yorg;
    double r    = 10;
    int    m    = 0;
    int    n    = 0;
                                            
    while( x<XMARGIN || x>=maxx || y<YMARGIN|| y>=maxy || !isNodePlaceable(node,x,y) )
    {
        x = (int)(xorg + r * Random::nextGaussian());
        y = (int)(yorg + r * Random::nextGaussian());
        ++n;
        if( n>4 )
        {
            n = 0;
            if(r<m_problem->m_width/2)
                r*=2;
        }
        m++;
        if( m > 200 )
        {
            //printf("szivas\n");
            return;                               
        }
    }
    placeNode( node, x, y );
}

void LayoutSolution::placeNodeToRandomPos( int node )
{
    placeNodeNearPos( node, XMARGIN+Random::nextInt(m_problem->m_width-m_nodes[node].m_node->m_sx-2*XMARGIN),
            YMARGIN+Random::nextInt(m_problem->m_height-m_nodes[node].m_node->m_sy-2*YMARGIN) );
}

void LayoutSolution::randomSwap()
{
    int node1 = Random::nextInt(m_nodes.size());
    int node2 = Random::nextInt(m_nodes.size());
    int x1    = m_nodes[node1].m_x;
    int y1    = m_nodes[node1].m_y;
    int x2    = m_nodes[node2].m_x;
    int y2    = m_nodes[node2].m_y;        
    removeNode(node1);
    removeNode(node2);
    placeNodeNearPos( node1, x2, y2 );
    placeNodeNearPos( node2, x1, y1 );
}

void LayoutSolution::moveOneToRandomPos()
{
    int node = Random::nextInt(m_nodes.size());
    removeNode( node );
    placeNodeToRandomPos( node );
}

void LayoutSolution::selectRandomNodes( IntSet& nodes )
{
    int n = Random::nextInt( m_nodes.size()/2 );
    for( int i=0; i<n; ++i )
    {
        int nodeInd = Random::nextInt( m_nodes.size() );
        nodes.insert( nodeInd );
    }
}

void LayoutSolution::addSubGraphNodes( Node * act, IntSet& nodes, Node * prohibited, double cutoff )
{
    //if( act == prohibited || Random::nextDouble() < cutoff || nodes.find( act->m_nodeId ) != nodes.end() )
    if( act == prohibited || nodes.find( act->m_nodeId ) != nodes.end() )
        return;
   
    nodes.insert( act->m_nodeId );

    for( int i=0; i<act->m_edges.size(); ++i )
    {
        Edge * e = act->m_edges[i];

        Node * n1 = e->getTopLevelFrom();
        Node * n2 = e->getTopLevelTo();

        if( n1!=act && n1!=prohibited )
            addSubGraphNodes( n1, nodes, prohibited, cutoff );
        if( n2!=act && n2!=prohibited )
            addSubGraphNodes( n2, nodes, prohibited, cutoff );
    }
}

void LayoutSolution::selectSubGraph( IntSet& nodes )
{
    Edge * edge = m_problem->m_edges[Random::nextInt(m_problem->m_edges.size())];

    Node * act;
    Node * prohibited;

    if( Random::nextDouble() < 0.5 )
    {
        act        = edge->getTopLevelFrom();
        prohibited = edge->getTopLevelTo();
    }
    else
    {
        prohibited = edge->getTopLevelFrom();
        act        = edge->getTopLevelTo();
    }

    double cutoff = 0;
    //if( Random::nextDouble() < 0.5 )
      //  cutoff = 0.5 * Random::nextDouble();

    addSubGraphNodes( act, nodes, prohibited, cutoff );
}

void LayoutSolution::moveSome()
{
    IntSet nodes;
    if( Random::nextDouble() < 0.2 )
        selectRandomNodes( nodes );
    else;
        selectSubGraph( nodes );

    int dx, dy;
    double r = 1 + 10 * Random::nextDouble();
    
    if( Random::nextDouble() < 0.2 )
        r = 200;
           
    if( Random::nextDouble() < 0.333 )
    {
        dx = (int)(r * Random::nextGaussian());
        dy = 0;
    }
    else if( Random::nextDouble() < 0.5 )
    {
        dx = 0;
        dy = (int)(r * Random::nextGaussian());
    }
    else
    {
        dx = (int)(r * Random::nextGaussian());
        dy = (int)(r * Random::nextGaussian());
    }

    IntSet::iterator it;
    for( it=nodes.begin(); it!=nodes.end(); ++it )
    {
        int node = *it;
        m_nodes[node].m_tempx = m_nodes[node].m_x + dx;
        m_nodes[node].m_tempy = m_nodes[node].m_y + dy; 
        removeNode( node );    
    }

    for( it=nodes.begin(); it!=nodes.end(); ++it )
    {
        int node = *it;
        placeNodeNearPos( node, m_nodes[node].m_tempx, m_nodes[node].m_tempy );
    }
}

void LayoutSolution::moveOne()
{
    int nodeInd = Random::nextInt( m_nodes.size() );

    double r = 1 + 10 * Random::nextDouble();    
    if( Random::nextDouble() < 0.2 )
        r = 200;
           
    int dx = (int)(r * Random::nextGaussian());
    int dy = (int)(r * Random::nextGaussian());    
    int x  = m_nodes[nodeInd].m_x + dx;
    int y  = m_nodes[nodeInd].m_y + dy; 

    removeNode( nodeInd );
    placeNodeNearPos( nodeInd, x, y );
}

void LayoutSolution::mutate()
{
    int r = Random::nextInt(8);

    switch( r )
    {
    case 0:
    case 1:
        moveOneToRandomPos();
        break;
    case 2:
    case 3:
        moveOne();
        break;
    case 4:
    case 5:
        moveSome();
        break;
    case 6:
    case 7:
        randomSwap();
        break;
    }
}

double LayoutSolution::getScore()
{
    int    i,j;
    int    crossings     = 0;
    int    dirViolations = 0;
    double length        = 0;
    int    n             = m_problem->m_edges.size();
    int    m             = m_nodes.size();
    double weightPointX  = 0;
    double weightPointY  = 0;

    for( i=0; i<n; ++i )
    {
        Edge * e1 = m_problem->m_edges[i];
        calcEdgeEnds( e1 );
        dirViolations += getDirViolations( e1 );        
        double dx = (e1->x1 - e1->x2);
        double dy = (e1->y1 - e1->y2);

        //double d  = fabs(dx) + fabs(dy);
        double d  = sqrt(dx*dx + dy*dy);// + fabs(dx);
        //d = pow(d, 2);

        length += d;
    }

    for( i=0; i<n-1; ++i )
    {
        Edge * e1 = m_problem->m_edges[i];
        for( j=i+1; j<n; ++j )
        {
            Edge * e2 = m_problem->m_edges[j];
            if( areConnectionsCrossed( e1, e2 ) )
                crossings++;
        }
    }

    return -0.01*length - 1*crossings - 10*dirViolations;
}