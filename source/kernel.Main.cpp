/** 
 * The operating system kernel main class.
 * 
 * @author    Sergey Baigudin, sergey@baigudin.software
 * @copyright 2016-2017, Embedded Team, Sergey Baigudin
 * @license   http://embedded.team/license/
 */
#include "kernel.Main.hpp" 
#include "kernel.Configuration.hpp"
#include "kernel.Interrupt.hpp"
#include "kernel.Resource.hpp"
#include "kernel.Allocator.hpp"
#include "library.Heap.hpp"
#include "system.Main.hpp" 

namespace kernel
{
    /**
     * The method which will be stated first.
     * 
     * @return error code or zero.
     */   
    int32 Main::main()
    {
        kernel_ = NULL;    
        int32 stage = 0;
        int32 error = -1;
        ::api::Heap* heap = NULL;        
        const ::kernel::Configuration config = ::Configuration();
        do
        {
            // Stage 1: construct a heap object and set the kernel heap allocator
            void* addr = config.heapAddr;
            int64 size = config.heapSize;
            if(addr == NULL || size <= 0) break;
            heap = new (addr) ::library::Heap(size);
            if(heap == NULL || not heap->isConstructed()) break;
            ::kernel::Allocator::setHeap(*heap);       
            // Stage 2
            stage++;
            if( not ::kernel::Interrupt::initialize() ) break;  
            // Stage 3: Create the kernel resource factory
            stage++;
            Resource kernel(config, *heap);            
            kernel_ = &kernel;
            if(kernel_ == NULL || not kernel_->isConstructed()) break;   
            // Stage complete
            stage = -1;
            error = ::system::Main::main(*kernel_);
        }
        while(false);
        switch(stage)
        {
            default:
            case 3:
            
            case 2: 
                ::kernel::Interrupt::deinitialize();              
                
            case 1: 
                
            case 0: 
                break;
        } 
        return error;
    }
    
    /**
     * Returns the kernel factory resource.
     *        
     * @return the kernel interface.
     */
    ::api::Kernel& Main::getKernel()
    {
        if(kernel_ == NULL) Interrupt::disableAll();
        return *kernel_;
    }            
    
    /**
     * The kernel factory resource (no boot).
     */
    ::api::Kernel* Main::kernel_;   

}   

/**
 * The main function.
 * 
 * The method should be called after default boot initialization, and
 * the following tasks must be:
 * 1. Stack has been set.
 * 2. CPU registers have been set.
 * 3. Run-time initialization has been completed.
 * 4. Global variables have been set.
 * 5. Global constructors have been called.
 * 
 * @return error code or zero.
 */   
int os_main(void* arg)
{
    return static_cast<int>( ::kernel::Main::main() );
}
