#ifndef _GMEGRAPH_H_
#define _GMEGRAPH_H_

#include "Graph.h"
#include "ComponentLib.h"

class GMEGraph : public Graph
{
public:
            GMEGraph           ( IMgaProject *project, IMgaModel* model, IMgaMetaAspect* aspect );

           ~GMEGraph           ();

private:
    void    fillNodes          ( IMgaProject *project, IMgaParts* parts );

    void    fillConnections    ( IMgaProject *project, IMgaParts* parts );

    Node *  findFCO            ( IMgaFCO * fco );

    void    setRoutingPrefs    ( Edge * edge, CString connPrefs );

};

#endif // _GMEGRAPH_H_
