#include <boost/ut.hpp>

// boost::ut relies on static destructors so if
// we don't defer LLVM coverage report until after
// ut's destructor we won't get any coverage.
#if defined(__llvm__) && defined(ANGONOKA_COVERAGE)
extern "C" {
static int __llvm_profile_runtime;
void __llvm_profile_initialize_file(void);
int __llvm_profile_write_file(void);
};
namespace {
struct on_exit {
    ~on_exit() noexcept
    {
        __llvm_profile_initialize_file();
        __llvm_profile_write_file();
    };
};
constinit on_exit _;
}; // namespace
#endif

int main() { }
