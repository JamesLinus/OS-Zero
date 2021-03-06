#define __ASSEMBLER__ 1
#include <kern/conf.h>
#include <kern/unit/x86/boot.h>
#include <kern/unit/x86/trap.h>
#include <kern/unit/x86/bits/cpu.h>
#include <kern/unit/ia32/macro.S>
#include <kern/unit/ia32/task.h>

/*
 * NOTE: we use interrupt gates, which automatically disable further
 * interrupts.
 */

/*
 * FIXME
 * - make the interrupt handlers add to interrupt mask, not set it
 * - use local APIC instead of PIC + PIT
 */

/*
 * CPU pushes EFLAGS, CS, and EIP, as well as an ERROR-code word if used.
 * return with IRET after return from handler.
 *
 * top of stack after interrupt
 * 
 * eflags
 * cs
 * eip		<- ESP if error code not present
 * (err)	<- ESP if error code supplied
 */

/*
 * interrupt prologue
 * ------------------
 * ESP points to IRET stack frame OR ERROR-code right below it.
 * push general-purpose registers with PUSHA.
 * possible ERROR-code is stored EDX.
 */

/*
 * interrupt epilogue
 * ------------------
 * interrupt routines with error must adjust stack pointer.
 * in the end, we enable interrupts with STI.
 */

//.extern	irqtimerfired

.globl	trapde, trapdb, trapnmi, trapbp, trapof, trapbr, trapud, trapnm, trapdf
.globl	trapts, trapnp, trapss, trapgp, trappf, trapmf, trapac, trapmc, trapxf
.globl	irqtmr0, irqtmr, irqkbd, irqmouse, irqerror, irqspurious
.globl	irqvec
#if (SMP) || (APIC)
.globl	irqerror, irqspurious, mpspurint, irqtmrcnt
#endif

.extern	sigfunc, tmrsyncscr, schedpicktask
.extern vmpagefault, kbdtrap, mousetrap
#if (APIC)
.extern	mpapic
#endif

.text	32

.align	16

trapde:
	pushl	$0
	pushl	$TRAPDE
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPDE, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPDE
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapdb:
	pushl	$0
	pushl	$TRAPDB
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPDB, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPDB
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapnmi:
	pushl	$0
	pushl	$TRAPNMI
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPNMI, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPNMI
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapbp:
	pushl	$0
	pushl	$TRAPBP
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPBP, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPBP
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapof:
	pushl	$0
	pushl	$TRAPOF
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPOF, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPOF
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapbr:
	pushl	$0
	pushl	$TRAPBR
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPBR, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPBR
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapud:
	pushl	$0
	pushl	$TRAPUD
	_trapenter
	movl	%esp, %ebp

	movl	8(%esp), %eax	// old ESP
	movl	(%eax), %ecx	// stored EIP
#if (FASTINTR)
	pushl	%eax
	movl	$TRAPUD, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPUD
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapnm:
	pushl	$0
	pushl	$TRAPNM
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPNM, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPNM
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapdf:
	pushl	$TRAPDF
	_trapentererr
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	/* error code is in %ecx */
	pushl	%ebx			// push context/frame argument
	movl	$TRAPDF, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPDF
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapts:
	pushl	$TRAPTS
	_trapentererr
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	/* error code is in %ecx */
	pushl	%ebx			// push context/frame argument
	movl	$TRAPTS, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPTS
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

	/* error code is segment selector index */
trapnp:
	pushl	$TRAPNP
	_trapentererr
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	/* error code is in %ecx */
	pushl	%ebx			// push context/frame argument
	movl	$TRAPNP, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPNP
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

	/* error code is segment selector index if not present, 0 otherwise */
trapss:
	pushl	$TRAPSS
	_trapentererr
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	/* error code is in %ecx */
	pushl	%ebx			// push context/frame argument
	movl	$TRAPSS, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPSS
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

	/* error-code is segment selector index if segment-related or 0 */
trapgp:
	pushl	$TRAPGP
	_trapentererr
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	/* error code is in %ecx */
	pushl	%ebx
	movl	$TRAPGP, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPGP
	pushl	$0x00000000
	pushl	%eax
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

_gpfin:	
	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

	/*
	 * - cr2 has virtual fault address
	 * - bit 0 of error 1 if protection violation, 0 if non-present page
	 * - bit 1 of error 1 if write, 0 if read
	 * - bit 2 of error 1 if CPL was 3
	 * - bit 3 of error 1 if 1 present in a reserved field
	 * - bit 4 set if caused by an instruction fetch
	 */
trappf:
	pushl	$TRAPPF
	_trapentererr
	movl	%esp, %ebp

#if (FASTINTR)
	leal	40(%esp), %ebx		// stack frame argument on stack
	/* error code is in %ecx */
	pushl	%ebx
	movl	%cr2, %edx		// fault address
	xorl	%gs:16, %eax		// FIXME: PID
#else
	movl	%cr2, %edx		// linear address
	pushl	%ecx			// error code
	pushl	%edx
	pushl	$0x00000000		// FIXME: push PID here
#endif	
	call	vmpagefault

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapmf:
	pushl	$0
	pushl	$TRAPMF
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPMF, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPMF
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapac:
	pushl	$0
	pushl	$TRAPAC
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPAC, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPAC
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapmc:
	pushl	$0
	pushl	$TRAPMC
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPMC, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPMC
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

trapxf:
	pushl	$0
	pushl	$TRAPXF
	_trapenter
	movl	%esp, %ebp

#if (FASTINTR)
	movl	8(%esp), %ebx		// stack frame argument on stack
	xorl	%ecx, %ecx		// zero third argument
	pushl	%ebx			// push context/frame argument
	movl	$TRAPXF, %edx
	xorl	%eax, %eax
#else
	pushl	%ecx
	pushl	$TRAPXF
	pushl	$0x00000000
#endif
	pushl	%esp			// ctx-argument for sigfunc
	call	sigfunc

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

#if (APIC)
	/* dummy interrupt handler for probing APIC frequency */
irqtmrcnt:
	_apiceoi
	iretl
#endif

	/*
	 * simple interface to let the PIT do sleep() and such actions early in
	 * the bootstrap process
	 */
irqtmr0:
	/* push general purpose registers */
	_trapenter
//	pushal
	/* store stack pointer */
	movl	%esp, %ebp
	/* load interrupt vector address */
	movl	$irqvec, %eax
	/* load interrupt handler address */
	movl	0(%eax), %ebx
	/* call if non-zero */
	cmpl	$0, %ebx
	jz	_tmr0fin
	call	*%ebx
	/* zero interrupt handler address */
	movl	$irqvec, %eax
	movl	$0x00000000, 0(%eax)

_tmr0fin:
	_piceoi1
	movl	%ebp, %esp
	/* pop general purpose registers */
	popal
	/* re-enable interrupts */
	sti
	/* return */
	iretl

	/* 32-bit x86 context switch */
	/*
	 * registers after interrupt
	 * -------------------------
	 * ebp		- old ESP _and_ EBP
	 *
	 * top of stack after prologue
	 * ---------------------------
	 * ss		- user-stack segment selector
	 * esp		- userland stack pointer
	 * eflags
	 * cs
	 * eip
	 * ebp		<- EBP, ESP
	 * eax
	 *
	 * NOTE: ss and esp are only pushed if the interrupt happened
	 * in user-mode, i.e. there's a privilege transition to system-mode
 	 */

.align	16

irqtmr:
	pushl	%ebp			// push frame pointer
	movl	%esp, %ebp		// store stack pointer
	pushl	%eax			// back %eax up
	movl	%gs:8, %eax		// per-cpu current task
	// adjust sp for pushing m_tcb.genregs
	leal	52(%eax), %esp
	pushal				// push general-purpose registers
	/* store FPU context */
	/* %esp points to top of segregs */
	movl	(%eax), %ebx		// fetch flg
	leal	128(%eax), %ecx		// ECX = &task->fpstate
	andl	$CPUHASFXSR, %ebx	// (flg & CPUHASFXSR) ?
	cmpl	$0, %ebx		// if so, branch to _fxsave
	jne	_fxsave			//
	fnsave	(%ecx)			// use fnsave instead
	jmp	_fpusaved
_fxsave:	
	fxsave	(%ecx)
_fpusaved:
	/* push segment registers */
	pushl	%gs			// kernel thread-local storage
	pushl	%fs			// user thread-local storage?
	pushl	%es			// buffer segment? something... :)
	pushl	%ds			// data segment
	/* %esp points to m_task->tcb */
	movl	8(%ebp), %edx		// EDX = pushed %cs
	movl	(%ebp), %ebx		// EBX = stored frame pointer
	movl	%cr3, %ecx		// ECX = %cr3 (PDBR)
	cmpl	$UTEXTSEL, %edx		// check for user-mode interrupt
	jne	_getsysstk		// stack-info was not pushed if not
	movl	$UDATASEL, %esi		// ESI = user data-segment selector
	jmp	_havestk		// determined stack parameters
_getsysstk:
	/* construct stack information */
	movl	$DATASEL, %esi		// ESI = system stack/data selector
_havestk:	
	fwait
	leal	4(%ebp), %edi		// EDI = trap frame addr
	movl	%ecx, 52(%eax)		// task->pdbr = PDBR
	movl	%edx, 68(%eax)		// task->frame.cs = caller %cs
	movl	%esi, 80(%eax)		// task->frame.ss = SS
	movl	-4(%ebp), %ecx		// ECX = pushed %eax
	movl	12(%ebp), %edx		// EDX = pushed flags register
	movl	%ebx, 76(%eax)		// patch stack pointer in m_tcb.frame
	movl	4(%ebp), %esi		// ESI = pushed return address
	movl	%ecx, 48(%eax)		// patch %eax in m_tcb.genregs
	movl	%edx, 72(%eax)		// frame->eflags = pre-interrupt value
	movl	%esi, 64(%eax)		// frame.eip = return address
	movl	%edi, 84(%eax)		// m_tcb.trapesp = trap frame address
	/* schedule next thread */
	//	movl	%gs:8, %eax		// task structure address
	/* task structure address is still in %eax */
	movl	%ebx, 28(%eax)		// task->genregs.ebp = pre-trap EBP
	movl	schedpicktask, %ecx	// get scheduler-specific function
	leal	4(%ebp), %esp		// restore stack pointer
	pushl	%eax			// store copy of task pointer
	call	*%ecx			// call scheduler function (taskpick)
	popl	%edx			// pop earlier task pointer
	cmpl	%eax, %edx		// did we reschedule the same task?
	jne	_swtch			// if not, branch to _swtch
	/* continue the same task */
	leal	20(%eax), %esp
	popal
	movl	32(%esp), %esp		// ESP = m_tcb.trapesp
#if (APIC)
	_apiceoi
#else
	_piceoi1
#endif
	iretl
_swtch:
	call	m_taskjmp
	
.align	16

irqkbd:
	pushl	$0
	pushl	$IRQKBD
	_trapenter
	movl	%esp, %ebp

	movl	$irqvec, %eax
	movl	4(%eax), %ebx
	jz	_irqkbddone
	call	*%ebx
_irqkbddone:	
#if (APIC)
	_apiceoi
#else
	_piceoi1
#endif
	
	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

irq3:
	pushl	$0
	pushl	$IRQCOM2AND4
	_trapenter
	movl	%esp, %ebp
#if (APIC)
	_apiceoi
#else
	_piceoi1
#endif
	
	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

irq4:
	pushl	$0
	pushl	$IRQCOM1AND3
	_trapenter
	movl	%esp, %ebp

#if (APIC)
	_apiceoi
#else
	_piceoi1
#endif
	
	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

irq5:
	pushl	$0
	pushl	$IRQLPT
	_trapenter
	movl	%esp, %ebp

	movl	$irqvec, %eax
	movl	20(%eax), %ebx
	jz	_irq5done
	call	*%ebx
_irq5done:	
#if (APIC)
	_apiceoi
#else
	_piceoi1
#endif
	
	movl	%ebp, %esp
	_trapleave
	sti
	iretl

.align	16

irqmouse:
	pushl	$0
	pushl	$IRQMOUSE
	_trapenter
	movl	%esp, %ebp

	movl	$irqvec, %eax
	movl	48(%eax), %ebx
	jz	_irqmousedone
	call	*%ebx
_irqmousedone:	
#if (APIC)
	_apiceoi
#else
	_piceoi1
	_piceoi2
#endif
	
	movl	%ebp, %esp
	_trapleave
	sti
	iretl

#if (SMP) || (APIC)

.align	16

irqerror:
	sti
	iretl

.align	16

irqspurious:
	_trapenter
	movl	%esp, %ebp

	call	mpspurint

	movl	%ebp, %esp
	_trapleave
	sti
	iretl

#endif /* SMP || APIC */

.data

.align	NBPG

	/* interrupt vector */
	/* 0x00..0x1f	- IRQs 0-15 */
	/* 19		- IRQERROR */
	/* 29		- IRQIPI */
	/* 31		- IRQSPURIOUS */
irqvec:
	.space	(8 * NINTR)

