#ifndef GUIDCREATE_H
#define GUIDCREATE_H

class GuidCreate
{
public:
	static const char * format_str;
	static CComBSTR newGuid();
};

#endif //GUIDCREATE_H