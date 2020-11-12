#include <stddef.h>
#include <stdint.h>

extern "C" {
    #include "boot/handoff/handoff_memory.h"
}

namespace Thornhill {

    class ThornhillMemoryManager {
        
        public:
            static void initialize(HandoffMemoryMap handoffMemoryMap);

    };

}
