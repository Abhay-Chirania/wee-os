#include <sched.h>
extern tcb *current_tcb;

static inline void scheduler() __attribute__((always_inline));
static inline void context_switch() __attribute__((always_inline));

#ifdef weighted_round_robin
static inline void schd_w_rr() __attribute__((always_inline));
#elif defined round_robin
static inline void schd_rr() __attribute__((always_inline));
#endif

void SysTick_Handler(void)
{
	__disable_irq();
	scheduler();
	__enable_irq();
}

inline void scheduler()
{
#ifdef weighted_round_robin
	schd_w_rr();
#elif defined round_robin
	schd_rr();
#endif
}

#ifdef weighted_round_robin
void schd_w_rr()
{
	if (current_tcb->c_weight!=0) {
		current_tcb->c_weight -= 1;
		__enable_irq();
		return;
	}
	context_switch();
	current_tcb->c_weight = current_tcb->weight;
	current_tcb = current_tcb->next_tcb;
	current_tcb->c_weight -= 1;
}
#elif defined round_robin
void schd_rr()
{
	context_switch();
	current_tcb = current_tcb->next_tcb;
}
#endif

inline void context_switch()
{
	__asm__ __volatile__ ("PUSH {R4-R11}");
        __asm__ __volatile__ ("LDR SP, %0" ::"m"(current_tcb->next_tcb->sp));
        __asm__ __volatile__ ("POP {R4-R11}");
}
