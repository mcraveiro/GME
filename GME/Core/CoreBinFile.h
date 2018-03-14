
#ifndef MGA_COREBINFILE_H
#define MGA_COREBINFILE_H

#include "CoreDictionaryAttributeValue.h"

#include <fstream>//fstream.h
#include <list>//slist
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

#include "windows.h"
class membuf
{
	public:
	membuf() : 
		begin(0), end(0), hFile(INVALID_HANDLE_VALUE), hFileMappingObject(INVALID_HANDLE_VALUE)
		{ }
	
	int open(const wchar_t* filename) {
		ASSERT(hFile == INVALID_HANDLE_VALUE);
		hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			return 1;
		}
		DWORD filesize = GetFileSize(hFile, NULL);
		if (filesize == INVALID_FILE_SIZE || filesize == 0) {
			return 1;
		}
		hFileMappingObject = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hFileMappingObject == INVALID_HANDLE_VALUE) {
			return 1;
		}
		begin = (char*)MapViewOfFile(hFileMappingObject, FILE_MAP_READ, 0, 0, 0);
		if (begin == NULL) {
			return 1;
		}
		end = begin + filesize;
		return 0;
	}

	~membuf() {
		if (begin)
			UnmapViewOfFile(begin);
		if (hFileMappingObject != INVALID_HANDLE_VALUE)
			CloseHandle(hFileMappingObject);
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}

	char* getBegin() const {
		return begin;
	}
	char* getEnd() const {
		return end;
	}

	private:
		membuf(const membuf&);
		membuf& operator=(const membuf&);

		HANDLE hFile,hFileMappingObject;
		char* begin, * end;
};

class CCoreBinFile;
class BinAttrUnion;

// --------------------------- BinAttr
class __declspec(novtable) BinAttrBase
{
public:
	BinAttrBase() : attrid(ATTRID_NONE) { }
	BinAttrBase(attrid_type attrid) : attrid(attrid) { }
	virtual ~BinAttrBase() { }

	attrid_type attrid;

	static BinAttrBase *Create(BinAttrBase& attr, valtype_type valtype);

	virtual valtype_type GetValType() const NOTHROW = 0;
	virtual void Set(CCoreBinFile *binfile, VARIANT p) = 0;
	virtual void Get(CCoreBinFile *binfile, VARIANT *p) = 0;
	virtual void Write(CCoreBinFile *binfile) = 0;
	virtual void Read(CCoreBinFile *binfile) = 0;
	// virtual move constructor
	virtual void move(BinAttrUnion&& dest) = 0;
};

class __declspec(novtable) BinAttrUnion : public BinAttrBase
{
public:
	BinAttrUnion() { }
	explicit BinAttrUnion(attrid_type attrid) : BinAttrBase(attrid) { }
	virtual ~BinAttrUnion() { }

	virtual valtype_type GetValType() const NOTHROW { DebugBreak(); return 0; }
	virtual void Set(CCoreBinFile *binfile, VARIANT p) { DebugBreak(); }
	virtual void Get(CCoreBinFile *binfile, VARIANT *p) { DebugBreak(); }
	virtual void Write(CCoreBinFile *binfile) { DebugBreak(); }
	virtual void Read(CCoreBinFile *binfile) { DebugBreak(); }
	virtual void move(BinAttrUnion&& dest) { DebugBreak(); }

	// that is a subtype of BinAttrUnion
	// that must not be BinAttrUnion because of __declspec(novtable)
	BinAttrUnion(BinAttrUnion&& that) {
		// volatile to read the vtable of that
		BinAttrUnion* volatile that_ = &that;
		that_->move(std::move(*this));
	}

	BinAttrUnion& operator=(BinAttrUnion&& that) {
		BinAttrUnion* volatile that_ = &that;
		that_->move(std::move(*this));
		return *this;
	}
	BinAttrUnion(const BinAttrUnion& that) {
		// FIXME
	}
	// BinAttrUnion is guaranteed to have enough space to contain any BinAttr<*>
	// (Ideally we'd do union { char [sizeof(BinAttrBase) - sizeof(BinAttr<*>) }, but that requires method definitions for BinAttr<*>::* to be separated from the declaration)
#ifdef _DEBUG
	char pad[12 + 2 * sizeof(void*)];
#else
	char pad[4 + sizeof(void*)];
#endif
};

typedef std::vector<BinAttrUnion> binattrs_type;
typedef binattrs_type::iterator binattrs_iterator;

template<valtype_enum VALTYPE>
class BinAttr;


// --------------------------- BinObject

class BinObject
{
public:
	~BinObject() { DestroyAttributes(); }

	// binattrs actually contains elements of type BinAttr<*>
	binattrs_type binattrs;
	bool deleted;

	void HasGuidAndStatAttributes(GUID& guid, bool* p_statusFound, bool* p_oldRegFound);
	void CreateGuidAttributes( CCoreBinFile* p_bf);
	void CreateStatusAttribute( CCoreBinFile* p_bf);
	void UpgradeRegistryIfNecessary(CCoreBinFile* binFile);

	BinAttrBase *Find(attrid_type attrid)
	{
		binattrs_iterator i = binattrs.begin();
		binattrs_iterator e = binattrs.end();
		while( i != e && (i)->attrid != attrid )
			++i;

		if( i == e )
			HR_THROW(E_BINFILE);

		return &*i;
	}

	void CreateAttributes(ICoreMetaObject *metaobject);
	void DestroyAttributes();
	void Read(CCoreBinFile *binfile);
	void Write(CCoreBinFile *binfile);

	bool HasEmptyPointers() const;
};

typedef stdext::hash_map< metaobjidpair_type
                        , BinObject
                        , metaobjid2pair_hashfunc
                        > objects_type;
typedef objects_type::iterator objects_iterator;


// --------------------------- CCoreBinFile

class ATL_NO_VTABLE CCoreBinFile : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICoreStorage, &__uuidof(ICoreStorage), &__uuidof(__MGACoreLib)>,
	public ISupportErrorInfoImpl<&__uuidof(ICoreStorage)>,
	public CComCoClass<CCoreBinFile, &__uuidof(CoreBinFile)>
{
public:
	CCoreBinFile();
	~CCoreBinFile();

BEGIN_COM_MAP(CCoreBinFile)
	COM_INTERFACE_ENTRY(ICoreStorage)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

DECLARE_REGISTRY_RESOURCEID(IDR_COREBINFILE)

// ------- MetaProject

public:
	CComObjPtr<ICoreMetaProject> metaproject;
	bindata metaprojectid;

	STDMETHODIMP get_MetaProject(ICoreMetaProject **p);
	STDMETHODIMP put_MetaProject(ICoreMetaProject *p);

	void OpenMetaProject();
	void CloseMetaProject() NOTHROW;

// ------- MetaObject

public:
	CComObjPtr<ICoreMetaObject> metaobject;
	metaid_type metaid;

	STDMETHODIMP get_MetaObject(ICoreMetaObject **p);
	STDMETHODIMP put_MetaObject(ICoreMetaObject *p);

	STDMETHODIMP get_MetaID(metaid_type *p);
	STDMETHODIMP put_MetaID(metaid_type p);

	void OpenMetaObject();
	void CloseMetaObject() NOTHROW;

// ------- MetaAttribute

public:
	CComObjPtr<ICoreMetaAttribute> metaattribute;
	attrid_type attrid;

	STDMETHODIMP get_MetaAttribute(ICoreMetaAttribute **p);
	STDMETHODIMP put_MetaAttribute(ICoreMetaAttribute *p);

	STDMETHODIMP get_AttrID(attrid_type *p);
	STDMETHODIMP put_AttrID(attrid_type p);

	void OpenMetaAttribute();
	void CloseMetaAttribute() NOTHROW;

// ------- Ios

public:
	char* cifs;
	char* cifs_eof;

	std::ofstream ofs;

public:
#define CoreBinFile_read(a, size) if (size > cifs_eof - cifs) HR_THROW(E_FILEOPEN); memcpy(&a, cifs, size); cifs += size;
	void read(unsigned char &a) { CoreBinFile_read(a, sizeof(unsigned char)); }
	void read(short &a) { CoreBinFile_read(a, sizeof(short)); }
	void read(int &a) { CoreBinFile_read(a, sizeof(int)); }
	void read(long &a)  { CoreBinFile_read(a, sizeof(long)); }
	void read(double &a)  { CoreBinFile_read(a, sizeof(double)); }
	void read(CComBstrObj &a);
	void read(CComBstrObj &ss, char*& cifs);
	void read(bindata &a);
	void read(unsigned char*& b, int& len);
	void readstring(char*& pos);

	void write(unsigned char a) { ofs.write((const char*)&a, sizeof(unsigned char)); }
	void write(short a) { ofs.write((const char*)&a, sizeof(short)); }
	void write(int a) { ofs.write((const char*)&a, sizeof(int)); }
	void write(long a) { ofs.write((const char*)&a, sizeof(long)); }
	void write(double a) { ofs.write((const char*)&a, sizeof(double)); }
	void write(const CComBstrObj &a);
	void write(const bindata &a);
	void write(const unsigned char* a, int len);
	void write(const wchar_t* a, int len) {
		write((const unsigned char*)a, len * sizeof(wchar_t));
	}
	void writestring(const char* pos);
// ------- Attribute

public:
	typedef std::map<BinAttrBase*, CComVariant> undos_type;
	typedef undos_type::iterator undos_iterator;

	undos_type undos;

	STDMETHODIMP get_AttributeValue(VARIANT *p);
	STDMETHODIMP put_AttributeValue(VARIANT p);

// ------- Object

public:
	objects_type objects;
	objects_iterator opened_object;
	bool isEmpty;

	typedef core::hash_map<metaid_type, objid_type> maxobjids_type;
	typedef maxobjids_type::iterator maxobjids_iterator;

	maxobjids_type maxobjids;
	core::forward_list<objects_iterator> deleted_objects;
	core::forward_list<objects_iterator> created_objects;

	struct resolve_type
	{
		objects_iterator obj;
		attrid_type attrid;
		metaobjidpair_type idpair;
	};

	typedef core::forward_list<resolve_type> resolvelist_type;
	resolvelist_type resolvelist;

	void InitMaxObjIDs();

public:
	STDMETHODIMP OpenObject(objid_type objid);
	STDMETHODIMP CreateObject(objid_type *objid);
	STDMETHODIMP CloseObject() NOTHROW;

	STDMETHODIMP DeleteObject();
	STDMETHODIMP LockObject();

// ------- Project

public:
	std::wstring filename;
	bool read_only;
	bool intrans;
	bool modified;

	membuf file_buffer;
	bool IsOpened() const { return !filename.empty(); }
	bool InTransaction() const { return intrans; }

	void CancelProject() NOTHROW;
	void SaveProject(const std::wstring& origfname, bool keepoldname);
	void LoadProject();

public:
	STDMETHODIMP OpenProject(BSTR connection, VARIANT_BOOL *ro_mode);
	STDMETHODIMP CreateProject(BSTR connection);
	STDMETHODIMP CloseProject( VARIANT_BOOL abort = VARIANT_FALSE) NOTHROW;
	STDMETHODIMP SaveProject(BSTR connection, VARIANT_BOOL keepoldname);
	STDMETHODIMP DeleteProject() { return E_NOTIMPL; }

	STDMETHODIMP BeginTransaction();
	STDMETHODIMP CommitTransaction();
	STDMETHODIMP AbortTransaction();

	STDMETHODIMP get_StorageType(long *p);
};

// --------------------------- BinAttr<VALTYPE_LONG>

template<>
class BinAttr<VALTYPE_LONG> : public BinAttrBase
{
public:
	BinAttr() : a(0) { }

	long a;

	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_LONG; }
	virtual void Set(CCoreBinFile *binfile, VARIANT p)
	{ ASSERT( binfile != NULL ); binfile->modified = true; CopyTo(p, a); }

	virtual void Get(CCoreBinFile *binfile, VARIANT *p) { CopyTo(a, p); }
	virtual void Write(CCoreBinFile *binfile) { binfile->write(a); }
	virtual void Read(CCoreBinFile *binfile) { binfile->read(a); }

    BinAttr(BinAttr<VALTYPE_LONG>&& that) : BinAttrBase(that.attrid), a(that.a) { }
    virtual void move(BinAttrUnion&& dest) {
        new (&dest) BinAttr<VALTYPE_LONG>(std::move(*this));
    }
};

// --------------------------- BinAttr<VALTYPE_REAL>

template<>
class BinAttr<VALTYPE_REAL> : public BinAttrBase
{
public:
	BinAttr() : a(0) { }

	double a;

	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_REAL; }
	virtual void Set(CCoreBinFile *binfile, VARIANT p)
	{ ASSERT( binfile != NULL ); binfile->modified = true; CopyTo(p, a); }

	virtual void Get(CCoreBinFile *binfile, VARIANT *p) { CopyTo(a, p); }
	virtual void Write(CCoreBinFile *binfile) { binfile->write(a); }
	virtual void Read(CCoreBinFile *binfile) { binfile->read(a); }
	BinAttr(BinAttr<VALTYPE_REAL>&& that) : BinAttrBase(that.attrid), a(that.a) { }
	virtual void move(BinAttrUnion&& dest) {
		new (&dest) BinAttr<VALTYPE_REAL>(std::move(*this));
	}
};

// --------------------------- BinAttr<VALTYPE_STRING>

template<>
class BinAttr<VALTYPE_STRING> : public BinAttrBase
{
public:
	BinAttr() : pos(NULL) { }

	CComBstrObj a;
	// Lazy read: if pos is non-null, a is invalid and 
	//    pos points to the pascal-style UTF-8 string in the memory-mapped mga file
	char* pos;

	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_STRING; }
	virtual void Set(CCoreBinFile *binfile, VARIANT p)
	{ 
		ASSERT( binfile != NULL ); 
		binfile->modified = true; 
		CopyTo(p, a);
		pos = NULL;
	}

	virtual void Get(CCoreBinFile *binfile, VARIANT *p) {
		if (pos != NULL) {
			binfile->read(this->a, this->pos);
			this->pos = NULL;
		}
		CopyTo(a, p);
	}
	virtual void Write(CCoreBinFile *binfile) {
		if (pos != NULL) {
			binfile->read(this->a, this->pos);
			this->pos = NULL;
		}
		binfile->write(a);
	}
	virtual void Read(CCoreBinFile *binfile) {
		binfile->readstring(pos);
		// to disable lazy read:
		//binfile->read(a, pos);
		//pos = NULL;
	}
	BinAttr(BinAttr<VALTYPE_STRING>&& that) : BinAttrBase(that.attrid), a(std::move(that.a)), pos(that.pos) { }
	virtual void move(BinAttrUnion&& dest) {
		new (&dest) BinAttr<VALTYPE_STRING>(std::move(*this));
	}
};

// --------------------------- BinAttr<VALTYPE_BINARY>

template<class Type>
struct free_deleter {
	void operator()(Type* p) { free(p); }
};

template<>
class BinAttr<VALTYPE_BINARY> : public BinAttrBase
{
public:
	BinAttr() : data(0), need_free(false) { }
	virtual ~BinAttr() { if (need_free) free(data); }

	unsigned char* data;
	// memcpy: if lazy read, data is not guaranteed to be properly aligned for int*
	int get_len() const { int ret; memcpy(&ret, data, sizeof(int)); return ret; }
	void put_len(int len) { memcpy(data, &len, sizeof(len)); }
	__declspec(property(get=get_len, put=put_len)) int len;
	unsigned char* get_value() const { return (data + sizeof(len)); }
	__declspec(property(get=get_value)) unsigned char* value;
	bool need_free;

	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_BINARY; }
	virtual void Set(CCoreBinFile *binfile, VARIANT v)
	{
		ASSERT( binfile != NULL );
		binfile->modified = true;
		if( v.vt == (VT_I4 | VT_ARRAY) )
		{
			if (need_free) free(data);
			data = (unsigned char*)malloc(sizeof(len) + sizeof(long) * GetArrayLength(v));
			need_free = true;
			len = sizeof(long) * GetArrayLength(v);
			CopyTo(v, (long*)(value), (long*)(value) + len/sizeof(long));
		}
		else
		{
			if (GetArrayLength(v)==0)
			{
				if (need_free) free(data);
				data = 0;
			}
			else
			{
				int len = GetArrayLength(v);
				if (need_free) free(data);
				data = (unsigned char*) malloc(sizeof(this->len) + len);
				need_free = true;
				this->len = len;
				CopyTo(v, value, value + len);
			}
		}
	}

	virtual void Get(CCoreBinFile *binfile, VARIANT *p) { 
		if (data == 0) {
			unsigned char* pnull=NULL;
			CopyTo(pnull, pnull, p);
		} else
			CopyTo(value, value + len, p);
	}
	virtual void Write(CCoreBinFile *binfile) { 
		if (data)
		{
			if (!need_free)
			{
				// need to get data off the disk; the file is going away
				unsigned char* olddata = this->data;
				int len = this->len;
				this->data = (unsigned char*)malloc(sizeof(len) + len);
				this->need_free = true;
				this->len = len;
				memcpy(value, olddata+sizeof(len), len);
			}
			binfile->write(value, len);
		}
		else
			binfile->write((unsigned char*)NULL, 0);
	}
	virtual void Read(CCoreBinFile *binfile) { 
		data = (unsigned char*)binfile->cifs;
		int len = this->len;
		// to test without lazy read:
		//data = (unsigned char*)malloc(sizeof(len) + len);
		//need_free = true;
		//this->len = len;
		//memcpy(value, binfile->cifs+sizeof(len), len);
		binfile->cifs += sizeof(len) + len;
	}
	BinAttr(BinAttr<VALTYPE_BINARY>&& that) : BinAttrBase(that.attrid), data(that.data), need_free(that.need_free) { that.need_free = false; }
	virtual void move(BinAttrUnion&& dest) {
		new (&dest) BinAttr<VALTYPE_BINARY>(std::move(*this));
	}
};

// --------------------------- BinAttr<VALTYPE_DICT>

template<>
class BinAttr<VALTYPE_DICT> : public BinAttrBase
{
public:
	BinAttr() : data(0) {
		CCoreDictionaryAttributeValue *val = NULL;
		typedef CComObject< CCoreDictionaryAttributeValue > COMTYPE;
		HRESULT hr = COMTYPE::CreateInstance((COMTYPE **)&val);
		COMTHROW(hr);
		dict = val;
	}
	virtual ~BinAttr() { }

	char* data;
	// memcpy: if lazy read, data is not guaranteed to be properly aligned for int*
	int read_len(char*& offset) const { int ret; memcpy(&ret, offset, sizeof(int)); offset += sizeof(int); return ret; }

	CComPtr<ICoreDictionaryAttributeValue> dict;

	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_DICT; }
	virtual void Set(CCoreBinFile *binfile, VARIANT v)
	{
		ASSERT( binfile != NULL );
		ASSERT(v.vt == VT_DISPATCH);
		binfile->modified = true;
		dict = 0;
		v.pdispVal->QueryInterface(&dict);
	}

	virtual void Get(CCoreBinFile *binfile, VARIANT *p) {
		if (dict == 0) {
			// lazy read
			CCoreDictionaryAttributeValue* val = NULL;
			typedef CComObject< CCoreDictionaryAttributeValue > COMTYPE;
			HRESULT hr = COMTYPE::CreateInstance((COMTYPE **)&val);
			COMTHROW(hr);

			char* data = this->data;
			int size = read_len(data);
			while (data < this->data + size)
			{
				int keysize = read_len(data);
				CComBSTR key(keysize / sizeof(wchar_t));
				memcpy(key.m_str, data, keysize);
				data += keysize;
				int valuesize = read_len(data);
				CComBSTR value(valuesize / sizeof(wchar_t));
				memcpy(value.m_str, data, valuesize);
				data += valuesize;
				val->m_dict.emplace(
					std::unordered_map<CComBSTR, CComBSTR, CComBSTR_Length>::value_type(std::move(key), std::move(value)));
			}

			this->dict = val;
			this->data = 0;
		}
		CComVariant ret = dict;
		COMTHROW(ret.Detach(p));
	}
	virtual void Write(CCoreBinFile *binfile) {
		int size = 0;

		if (dict == NULL)
		{
			// need to read before write, since the file is going away
			CComVariant p;
			Get(binfile, &p);
		}

		const CCoreDictionaryAttributeValue* cdict = (const CCoreDictionaryAttributeValue*)(const ICoreDictionaryAttributeValue*)dict;
		for (auto it = cdict->m_dict.begin(); it != cdict->m_dict.end(); it++) {
			size += sizeof(int);
			size += it->first.Length() * sizeof(wchar_t);
			size += sizeof(int);
			size += it->second.Length() * sizeof(wchar_t);
		}
		binfile->write(size);

		for (auto it = cdict->m_dict.begin(); it != cdict->m_dict.end(); it++) {
			// binfile->write((int)it->first.Length());
			binfile->write(it->first, it->first.Length());
			// binfile->write((int)it->second.Length());
			binfile->write(it->second, it->second.Length());
		}
	}
	virtual void Read(CCoreBinFile *binfile) { 
		dict = 0;
		data = (char*)binfile->cifs;
		int len = read_len(binfile->cifs);
		binfile->cifs += len;
	}
	BinAttr(BinAttr<VALTYPE_DICT>&& that) : BinAttrBase(that.attrid), data(that.data), dict(std::move(that.dict)) { }
	virtual void move(BinAttrUnion&& dest) {
		new (&dest) BinAttr<VALTYPE_DICT>(std::move(*this));
	}
};

// --------------------------- BinAttr<VALTYPE_LOCK>

template<>
class BinAttr<VALTYPE_LOCK> : public BinAttrBase
{
public:
	BinAttr() : a(0) { }

	lockval_type a;

	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_LOCK; }
	virtual void Set(CCoreBinFile *binfile, VARIANT p) { CopyTo(p, a); }
	virtual void Get(CCoreBinFile *binfile, VARIANT *p) { CopyTo(a, p); }
	virtual void Write(CCoreBinFile *binfile) { }
	virtual void Read(CCoreBinFile *binfile) { a = 0; }
	BinAttr(BinAttr<VALTYPE_LOCK>&& that) : BinAttrBase(that.attrid), a(that.a) { }
	virtual void move(BinAttrUnion&& dest) {
		new (&dest) BinAttr<VALTYPE_LOCK>(std::move(*this)); 
	}
};
// --------------------------- BinAttr<VALTYPE_COLLECTION>

template<>
class BinAttr<VALTYPE_COLLECTION> : public BinAttrBase
{
public:
	std::unique_ptr<std::vector<objects_iterator>> backing;
	std::vector<objects_iterator>& getbacking() const { return *backing.get(); }
	__declspec(property(get = getbacking )) std::vector<objects_iterator>& a;

	BinAttr() : backing(new std::vector<objects_iterator>()) { }
	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_COLLECTION; }
	virtual void Set(CCoreBinFile *binfile, VARIANT p) { ASSERT(false); }
	virtual void Get(CCoreBinFile *binfile, VARIANT *p)
	{
		ASSERT( p != NULL && p->vt == VT_EMPTY );

		std::vector<metaobjidpair_type> idpairs;

		std::vector<objects_iterator>::const_iterator i = a.begin();
		std::vector<objects_iterator>::const_iterator e = a.end();
		while( i != e )
		{
			idpairs.push_back( (*i)->first );

			++i;
		}

		CopyTo(idpairs, p);
	}
	virtual void Write(CCoreBinFile *binfile) { }
	virtual void Read(CCoreBinFile *binfile) { }
    BinAttr(BinAttr<VALTYPE_COLLECTION>&& that) : BinAttrBase(that.attrid), backing(std::move(that.backing)) { }
	virtual void move(BinAttrUnion&& dest) {
		new (&dest) BinAttr<VALTYPE_COLLECTION>(std::move(*this));
	}
};

// --------------------------- BinAttr<VALTYPE_POINTER>

template<>
class BinAttr<VALTYPE_POINTER> : public BinAttrBase
{
public:
	BinAttr() : isEmpty(true) { }

	objects_iterator a;
	bool isEmpty;

	virtual valtype_type GetValType() const NOTHROW { return VALTYPE_POINTER; }

	void Set(CCoreBinFile *binfile, objects_iterator b)
	{
		ASSERT( binfile != NULL );
		ASSERT( isEmpty ); 
		ASSERT( b != binfile->objects.end());

		binfile->modified = true;

		a = b;
		isEmpty = false;

		ASSERT( binfile->opened_object->second.Find(attrid) == this );

		BinAttrBase *base = a->second.Find(attrid + ATTRID_COLLECTION);
		ASSERT( base != NULL );
		
		ASSERT( base->GetValType() == VALTYPE_COLLECTION );
		std::vector<objects_iterator> &objs = ((BinAttr<VALTYPE_COLLECTION>*)base)->a;

	#ifdef DEBUG_CONTAINERS
		std::vector<objects_iterator>::iterator i = find(objs.begin(), objs.end(), a);
		ASSERT( i == objs.end() );
	#endif

		objs.push_back(binfile->opened_object);
	}

	virtual void Set(CCoreBinFile *binfile, VARIANT p)
	{
		if( !isEmpty )
		{
			BinAttrBase *base = a->second.Find(attrid + ATTRID_COLLECTION);
			ASSERT( base != NULL );
			
			ASSERT( base->GetValType() == VALTYPE_COLLECTION );
			std::vector<objects_iterator> &objs = ((BinAttr<VALTYPE_COLLECTION>*)base)->a;

			ASSERT( binfile->opened_object->second.Find(attrid) == this );

			std::vector<objects_iterator>::iterator i = std::find(objs.begin(), objs.end(), binfile->opened_object);
			ASSERT( i != objs.end() );

			objs.erase(i);
		}

		isEmpty = true;

		metaobjidpair_type idpair;
		CopyTo(p, idpair);

		if( idpair.metaid == METAID_NONE )
		{
			ASSERT( idpair.objid == OBJID_NONE );
		}
		else
		{
			ASSERT( idpair.objid != OBJID_NONE );

			Set(binfile, binfile->objects.find(idpair));
		}
	}
	virtual void Get(CCoreBinFile *binfile, VARIANT *p)
	{
		if( isEmpty )
		{
			metaobjidpair_type idpair;
			idpair.metaid = METAID_NONE;
			idpair.objid = OBJID_NONE;
			CopyTo(idpair, p);
		}
		else
			CopyTo(a->first, p);
	}

	virtual void Write(CCoreBinFile *binfile)
	{
		if( isEmpty )
		{
			binfile->write((metaid_type)METAID_NONE);
		}
		else
		{
			ASSERT( a->first.metaid != METAID_NONE );
			ASSERT( a->first.objid != OBJID_NONE );

			binfile->write((metaid_type)a->first.metaid);
			binfile->write((objid_type)a->first.objid);
		}
	}

	virtual void Read(CCoreBinFile *binfile)
	{
		ASSERT( isEmpty );

		metaid_type metaid;
		binfile->read(metaid);

		if( metaid != METAID_NONE )
		{
			objid_type objid;
			binfile->read(objid);

			ASSERT( objid != OBJID_NONE );

			binfile->resolvelist.push_front(CCoreBinFile::resolve_type());
			CCoreBinFile::resolve_type &b = binfile->resolvelist.front();

			ASSERT( !binfile->isEmpty );

			b.obj = binfile->opened_object;
			b.attrid = attrid;
			b.idpair.metaid = metaid;
			b.idpair.objid = objid;
		}
	}
	BinAttr(BinAttr<VALTYPE_POINTER>&& that) : BinAttrBase(that.attrid), a(std::move(that.a)), isEmpty(that.isEmpty) { }
	virtual void move(BinAttrUnion&& dest) {
		new (&dest) BinAttr<VALTYPE_POINTER>(std::move(*this));
	}
};


static_assert(sizeof(BinAttr<VALTYPE_LONG>) <= sizeof(BinAttrUnion), "BinAttrUnion is too small.");
static_assert(sizeof(BinAttr<VALTYPE_REAL>) <= sizeof(BinAttrUnion), "BinAttrUnion is too small.");
static_assert(sizeof(BinAttr<VALTYPE_STRING>) <= sizeof(BinAttrUnion), "BinAttrUnion is too small.");
static_assert(sizeof(BinAttr<VALTYPE_POINTER>) <= sizeof(BinAttrUnion), "BinAttrUnion is too small.");
static_assert(sizeof(BinAttr<VALTYPE_BINARY>) <= sizeof(BinAttrUnion), "BinAttrUnion is too small.");
static_assert(sizeof(BinAttr<VALTYPE_COLLECTION>) <= sizeof(BinAttrUnion), "BinAttrUnion is too small.");


#endif//MGA_COREBINFILE_H
