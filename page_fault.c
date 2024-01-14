#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * --------------------------------- PROBLEM 6 --------------------------------------
 * Checkout PDF section 7 for this problem
 * 
 * Page fault handler.
 * 
 * When the CPU encounters an invalid address mapping in a page table, it invokes the 
 * OS via this handler. Your job is to put a mapping in place so that the translation 
 * can succeed.
 * 
 * @param addr virtual address in the page that needs to be mapped into main memory.
 * 
 * HINTS:
 *      - You will need to use the global variable current_process when
 *      altering the frame table entry.
 *      - Use swap_exists() and swap_read() to update the data in the 
 *      frame as it is mapped in.
 * ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t addr) {
   // TODO: Get a new frame, then correctly update the page table and frame table

   vpn_t vpn = vaddr_vpn(addr);
   pte_t* pte_base = (pte_t*)(mem + PTBR * PAGE_SIZE);
   pte_t* pte = pte_base + vpn;

   pfn_t pfn = free_frame();

   pte->pfn = pfn;
   pte->valid = 1;
   pte->dirty = 0;

   fte_t* fte = frame_table + pfn;
   fte->mapped = 1;
   fte->process = current_process;
   fte->vpn = vpn;

   uint8_t* new_frame = mem + pfn * PAGE_SIZE;
   if(swap_exists(NULL)){
      memset(new_frame, 0, PAGE_SIZE);
   } else {
      swap_read(pte, new_frame);
   }
   stats.page_faults += 1;

}

#pragma GCC diagnostic pop
