#ifndef OclViolation_h
#define OclViolation_h

#include "OCLCommon.h"
#include "unknwn.h"
// cannot use _com_ptr_t instead of IUnknown *
// because of incluse mess
// CComPtr does not work in vectors

namespace std {

class PtrVector : public vector<IUnknown *> //z!
{
public:
	PtrVector() : vector<IUnknown *>()
	{
	}

	PtrVector(const PtrVector &vect)
	{
		copy(vect);
	}

	PtrVector & operator =(const PtrVector &vect)
	{
		copy(vect);
		return *this;
	}

	void copy(const PtrVector &vect)
	{
		int len = size();
		for (int k=0; k<len; k++)
		{
			IUnknown *ptr = (*this)[k];
			if (ptr)
			{
				ptr->Release();
				(*this)[k] = NULL;
			}
		}
		clear();
		size_t lenx = vect.size();
		for (size_t kk=0; kk<lenx; kk++)
		{
			IUnknown *ptr = vect[kk];
			if (ptr)
				ptr->AddRef();
			push_back(ptr);
		}
	}

	virtual ~PtrVector()
	{
		EraseAll();
	}

	void clear() 
	{
		EraseAll();
		vector<IUnknown *>::clear();
	}

	void pop_back()
	{
		IUnknown* element = back();
		if (element)
			element->Release();
		vector<IUnknown *>::pop_back();
	}

	iterator erase(iterator it)
	{
		if (*it)
			(*it)->Release();
		return vector<IUnknown *>::erase(it);

	}

	iterator erase(iterator first, iterator last)
	{
		for (iterator t = first; t != last; t++)
		{
			if (*t)
				(*t)->Release();
		}
		return vector<IUnknown *>::erase(first, last);
	}

	void EraseAll()
	{
		int len = size();
		for (int k=0; k<len; k++)
		{
			IUnknown *ptr = (*this)[k];
			if (ptr)
			{
				ptr->Release();
				(*this)[k] = NULL;
			}
		}
	}
};

}; // namespace std

namespace OclTree
{
	struct Violation {
		bool						bIsException;
		std::string					strMessage;
		std::string					strSignature;
		std::string					methodName;
		Position					position;
		StringVector				vecVariables;
		StringVector				vecObjects;
		std::PtrVector				vecObjectsPtr;
	};

	typedef std::vector< Violation >	ViolationVector;
}

#endif // OclViolation_h
