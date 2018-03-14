#ifndef GME_INTERPRETER_H
#define GME_INTERPRETER_H

#include "Builder.h"


#define NEW_BON_INVOKE
//#define DEPRECATED_BON_INVOKE_IMPLEMENTED

class CComponent {
public:
	CComponent() : focusfolder(NULL) { ; } 
	CBuilderFolder *focusfolder;
	CBuilderFolderList selectedfolders;
	void InvokeEx(CBuilder &builder,CBuilderObject *focus, CBuilderObjectList &selected, long param);
//	void Invoke(CBuilder &builder, CBuilderObjectList &selected, long param);
};

#endif // whole file
