#include <cstdint>

#ifndef LIBTH_KERNEL
#define LIBTH_KERNEL

namespace Thornhill {

class Kernel {

  public:
    static void panic(const char* reason, uint64_t interruptNumber = 69);
};

} // namespace Thornhill

#endif