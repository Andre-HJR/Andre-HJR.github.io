#include <iostream>
#include <vector>

using namespace std;

template <class P> class RefCounted {
    unsigned int* pCount_;
  protected:
    RefCounted(): pCount_(new unsigned int(1)) {}
    bool IsUnique() const { return *pCount_ == 1; }
    P Clone(const P& val) { ++*pCount_; return val; }
    bool Release(const P&) { if (!--*pCount_) delete pCount_; return false; }
    static void hello() { std::cout << "Hello RefCounted!\n"; }
    enum { destructiveCopy = false };
};

template <class P> class COMRefcounted {
  public:
    static P Clone(const P& val) { val->AddRef(); return val; }
    static bool Release(const P& val) { val->Release(); return false; }
    enum { destructiveCopy = false };
};

template <class T> class DefaultSPStorage {
  protected:
    typedef T* StoredType;
    typedef T* InitPointerType;
    typedef T* PointerType;
    typedef T& ReferenceType;
  public:
    DefaultSPStorage(): pointee_(Default()) {}
    DefaultSPStorage(const DefaultSPStorage& p) : pointee_(0) {}
    DefaultSPStorage(const StoredType& p) : pointee_(p) {}
    template<class U> DefaultSPStorage(const DefaultSPStorage<U>&): pointee_(0) {}


    PointerType operator->() const { return pointee_; }
    ReferenceType operator*() const { return *pointee_; }

    friend inline PointerType GetImpl(const DefaultSPStorage& sp) {
      return sp.pointee_;
    }
    friend inline const StoredType& GetImplRef (const DefaultSPStorage& sp) {
      return sp.pointee_;
    }
    friend inline StoredType& GetImplRef(DefaultSPStorage& sp) {
      return sp.pointee_;
    }
  protected:
    void Destroy() { delete pointee_; }
    static StoredType Default() { return 0; }
  private:
    StoredType pointee_;
};

struct AllowConversion {
  enum { allow = true };
  void Swap(AllowConversion) {}
};

struct DisallowConversion {
  DisallowConversion() {}
  DisallowConversion(const AllowConversion&) {}
  enum { allow = false };
  void Swap(DisallowConversion) {}
};

template <class P> struct NoCheck {
  NoCheck() {}
  template <class P1> NoCheck(const NoCheck<P1>&) {}
  static void OnDefault(const P&) {}
  static void OnInit(const P&) {}
  static void OnDeference(const P&) {}
  static void Swap(NoCheck) {}
};

template <class P> struct AssertCheck {
  AssertCheck() {}
  template <class P1> AssertCheck(const AssertCheck<P1>&) {}
  template <class P1> AssertCheck(const NoCheck<P1>&) {}
  static void Ondefault(const P&) {}
  static void OnInit(const P&) {}
  static void OnDereference(P val) { assert(val); (void)val; }
  static void Swap(AssertCheck) {}
};


// Default for SmartPtr
template <class T,
         template <class> class OwnershipPolicy = RefCounted, 
         class Conversionpolicy = DisallowConversion,
         template <class> class CheckingPolicy = AssertCheck,
         template <class> class Storage = DefaultSPStorage
>
class SmartPtr;

class CTest
{
public:
	CTest() : m_id(0) {}
	CTest(int id) : m_id(id) {}
	~CTest() { std::cout << "id: " << m_id << " - Destructor is being called/n"; }
	void SetId(int id) { m_id = id; }
	int GetId() { return m_id; }
	void DoSomething()
	{ std::cout << "id: " << m_id << " - Doing something/n"; }
private:
	int m_id;
};


// void PT(SmartPtr<CTest>& t) {
//   std::cout << "id: " << t->GetId() << std::endl;
// }

int main(int argc, const char* argv[]) {
  // std::vector<SmartPtr<CTest>> testVector;
  // SmartPtr<CTest> pTest0(new CTest(0));
  // SmartPtr<CTest> pTest1(new CTest(1));
  // CTest* PCTest = new CTest(0);
  return 0;
}
