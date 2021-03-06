// RUN: %check_clang_tidy %s misc-unused-using-decls %t -- -- -fno-delayed-template-parsing

// ----- Definitions -----
template <typename T> class vector {};
namespace n {
class A;
class B;
class C;
class D;
class D { public: static int i; };
template <typename T> class E {};
template <typename T> class F {};
class G { public: static void func() {} };
class H { public: static int i; };
class I {
 public:
  static int ii;
};
template <typename T> class J {};
class G;
class H;

class Base {
 public:
  void f();
};

D UsedInstance;
D UnusedInstance;

int UsedFunc() { return 1; }
int UnusedFunc() { return 1; }
template <typename T> int UsedTemplateFunc() { return 1; }
template <typename T> int UnusedTemplateFunc() { return 1; }
template <typename T> int UsedInTemplateFunc() { return 1; }
void OverloadFunc(int);
void OverloadFunc(double);
int FuncUsedByUsingDeclInMacro() { return 1; }

class ostream {
public:
  ostream &operator<<(ostream &(*PF)(ostream &));
};
extern ostream cout;
ostream &endl(ostream &os);
}

// ----- Using declarations -----
// eol-comments aren't removed (yet)
using n::A; // A
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: using decl 'A' is unused
// CHECK-FIXES: {{^}}// A
using n::B;
using n::C;
using n::D;
using n::E; // E
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: using decl 'E' is unused
// CHECK-FIXES: {{^}}// E
using n::F;
using n::G;
using n::H;
using n::I;
int I::ii = 1;
class Derived : public n::Base {
 public:
  using Base::f;
};
using n::UsedInstance;
using n::UsedFunc;
using n::UsedTemplateFunc;
using n::UnusedInstance; // UnusedInstance
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: using decl 'UnusedInstance' is unused
// CHECK-FIXES: {{^}}// UnusedInstance
using n::UnusedFunc; // UnusedFunc
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: using decl 'UnusedFunc' is unused
// CHECK-FIXES: {{^}}// UnusedFunc
using n::cout;
using n::endl;

using n::UsedInTemplateFunc;
using n::J;
template <typename T> void Callee() {
  J<T> j;
  UsedInTemplateFunc<T>();
}

using n::OverloadFunc; // OverloadFunc
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: using decl 'OverloadFunc' is unused
// CHECK-FIXES: {{^}}// OverloadFunc

#define DEFINE_INT(name)        \
  namespace INT {               \
  static const int _##name = 1; \
  }                             \
  using INT::_##name
DEFINE_INT(test);
#undef DEFIND_INT

#define USING_FUNC \
  using n::FuncUsedByUsingDeclInMacro;
USING_FUNC
#undef USING_FUNC

namespace N1 {
// n::G is used in namespace N2.
// Currently, the check doesn't support multiple scopes. All the relevant
// using-decls will be marked as used once we see an usage even the usage is in
// other scope.
using n::G;
}

namespace N2 {
using n::G;
void f(G g);
}

void IgnoreFunctionScope() {
// Using-decls defined in function scope will be ignored.
using n::H;
}

// ----- Usages -----
void f(B b);
void g() {
  vector<C> data;
  D::i = 1;
  F<int> f;
  void (*func)() = &G::func;
  int *i = &H::i;
  UsedInstance.i;
  UsedFunc();
  UsedTemplateFunc<int>();
  cout << endl;
}
