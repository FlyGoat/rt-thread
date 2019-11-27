	.section .exc_vectors, "ax", %progbits
	.extern tlb_refill_handler
	.extern cache_error_handler
	.extern mips_irq_handle

	/* Exception Handler */

	/* 0x0 - TLB refill handler */
	.org 0x0
	.global tlb_refill_exception
	.type	tlb_refill_exception,@function
tlb_refill_exception:
	j	tlb_refill_handler
	nop
	
	/* 0x100 - Cache error handler */
	.org 0x100
	j	cache_error_handler
	nop
    
	/* 0x180 - Exception/Interrupt handler */
	.org 0x180
	.global general_exception
	.type	general_exception,@function
general_exception:
	j	_general_exception_handler
	nop
    
	/* 0x200 - Special Exception Interrupt handler (when IV is set in CP0_CAUSE) */
	.org 0x200
	.global irq_exception
	.type	irq_exception,@function
irq_exception:
	j	_irq_handler
	nop

	/* general exception handler */
_general_exception_handler:
	.set	noreorder
	j	mips_irq_handle
	nop
	.set	reorder

	/* interrupt handler */
_irq_handler:
	.set	noreorder
	j	mips_irq_handle
	nop
	.set	reorder
