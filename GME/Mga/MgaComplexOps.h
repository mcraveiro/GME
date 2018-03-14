#ifndef MGACOMPLEXOPS_H_1093481093485091384
#define MGACOMPLEXOPS_H_1093481093485091384

class PointerFixup;

void CheckConflict(CoreObj &b, CoreObj &root);
int GetRealSubtypeDist(CoreObj oldobj);

void ObjTreeCopy(CMgaProject *mgaproject, CoreObj self, CoreObj &nobj, coreobjpairhash &crealist);
void ObjTreeCopyFoldersToo(CMgaProject *mgaproject, CoreObj self, CoreObj &nobj, coreobjpairhash &crealist);

void ObjTreeCopyFromExt(CMgaProject *mgaproject, const CoreObj &orig, CoreObj &nobj, PointerFixup &fixup);

void ObjTreeCollect(CMgaProject *mgaproject, CoreObj &self, coreobjhash &crealist, int code );
void ObjTreeCollectFoldersToo(CMgaProject *mgaproject, CoreObj &self, coreobjhash &crealist, int code );

void ObjTreeDist(CoreObj self, int derdist);
void ReDeriveNewObjs(CMgaProject *mgaproject, std::vector<CoreObj> &orignobjs, int cnt, int targetlevel);
void shiftlist(coreobjpairhash &list1, coreobjhash &list2);

void ObjTreeCheckRelations(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals);
void ObjTreeCheckRelationsFoldersToo(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals);

void ObjTreeCheckINTORelations(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals);
void ObjTreeCheckINTORelationsFoldersToo(CMgaProject *mgaproject, CoreObj &self, coreobjhash &internals);

// the counterpart of this is declared in the MgaFCO.h
bool ObjTreeReconnectFoldersToo(CoreObj self, coreobjpairhash &crealist, CoreObj const &derivtgt = NULLCOREOBJ);

void DeriveMoveds(CMgaProject *mgaproject, std::vector<CoreObj> &orignobjs, std::vector<int> &extmoved, int cnt, int targetlevel);

void SingleObjTreeDelete(CoreObj &self, bool deleteself = true); 
void assignGuid( CMgaProject *mgaproject, CoreObj& ss);
void getMeAGuid( long *p_l1, long *p_l2, long *p_l3, long *p_l4);

#endif //MGACOMPLEXOPS_H_1093481093485091384
// created by ZolMol