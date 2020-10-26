#include <stddef.h>
#include <stdint.h>

namespace ThornhillMemory {

    class Physical {

        private:
            static uint8_t MEMORY[1024 * 1024 / 8];

            static size_t totalMemory;
            static size_t usedMemory;
        
        public:
            //
            // Getters
            //

            static size_t getTotalMemory() {
                return totalMemory;
            }

            static size_t getUsedMemory() {
                return usedMemory;
            }


            //
            // Memory Management Functions
            //

            static void allocate(size_t memorySize);

    };

}