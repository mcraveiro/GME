#include <xercesc/dom/dom.hpp>
#include <string>

template <class X>
class xml_smart_ptr
{
private:
	X* p;
public:
	xml_smart_ptr(X* p=0) : p(p) {}
	~xml_smart_ptr() { XERCES_CPP_NAMESPACE::XMLString::release(&p); }
	operator X*() const { return p; }
private: // noncopyable
	xml_smart_ptr(const xml_smart_ptr<X>&);
	const xml_smart_ptr<X>& operator=(const xml_smart_ptr<X>&);
};
typedef xml_smart_ptr<XMLCh> smart_XMLCh;
typedef xml_smart_ptr<char> smart_Ch;

static std::string operator+(const std::string& left, const smart_Ch& right) {
	return left + static_cast<const char*>(right);
}
