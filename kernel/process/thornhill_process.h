#ifndef TH_PROCESS_H
#define TH_PROCESS_H

typedef enum {
    CPU_PRIV_LEVEL_KERNEL = 0,
    CPU_PRIV_LEVEL_HIGH = 1,
    CPU_PRIV_LEVEL_LOW = 2,
    CPU_PRIV_LEVEL_USER = 3
} CpuPrivilegeLevel;

#endif