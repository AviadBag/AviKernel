#ifndef _PHYSICAL_MGR
#define _PHYSICAL_MGR

typedef void* physical_addr;

class PhysicalMgr
{
    public:
        PhysicalMgr();

        physical_addr allocate_block();
        void free_block(physical_addr);
};

#endif