/*
 * Author: Landon Fuller <landonf@plausiblelabs.com>
 *
 * Copyright (c) 2008-2013 Plausible Labs Cooperative, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PLCRASH_ASYNC_THREAD_H
#define PLCRASH_ASYNC_THREAD_H

#include <sys/ucontext.h>
#include "PLCrashAsync.h"

/**
 * @internal
 * @ingroup plcrash_async_thread
 * @{
 */

/* Configure supported targets based on the host build architecture. There's currently
 * no deployed architecture on which simultaneous support for different processor families
 * is required (or supported), but -- in theory -- such cross-architecture support could be
 * enabled by modifying these defines. */
#if defined(__i386__) || defined(__x86_64__)
#define PLCRASH_ASYNC_THREAD_X86_SUPPORT 1
#include <mach/i386/thread_state.h>
#endif

#if defined(__arm__)
#define PLCRASH_ASYNC_THREAD_ARM_SUPPORT 1
#include <mach/arm/thread_state.h>
#endif

/**
 * Target-neutral thread-state union.
 *
 * Thread state union large enough to hold the thread state for any supported
 * architecture.
 */
typedef union plcrash_async_thread_state {
#ifdef PLCRASH_ASYNC_THREAD_ARM_SUPPORT
    struct {
        /** ARM thread state */
        arm_thread_state_t thread;
    } arm_state;
#endif
    
#ifdef PLCRASH_ASYNC_THREAD_X86_SUPPORT
    /** Combined x86 32/64 thread state */
    struct {
        /** Thread state */
        x86_thread_state_t thread;
        
        /** Exception state. */
        x86_exception_state_t exception;
    } x86_state;
#endif
} plcrash_async_thread_state_t;

/** Register number type */
typedef int plcrash_regnum_t;

/**
 * General pseudo-registers common across platforms.
 *
 * Platform registers must be allocated starting at a 0
 * index, with no breaks. The following pseudo-register
 * values must be valid, and unused by other registers.
 */
typedef enum {
    /** Instruction pointer */
    PLCRASH_REG_IP = 0,
    
    /** Frame pointer */
    PLCRASH_REG_FP = 1,
} plcrash_gen_regnum_t;


#import "PLCrashAsyncThread_x86.h"
#import "PLCrashAsyncThread_arm.h"


/** Platform word type */
typedef plcrash_pdef_greg_t plcrash_greg_t;

void plcrash_async_thread_state_ucontext_init (plcrash_async_thread_state_t *thread_state, ucontext_t *uap);
plcrash_error_t plcrash_async_thread_state_mach_thread_init (plcrash_async_thread_state_t *thread_state, thread_t thread);


/* Platform specific funtions */

/**
 * Get a register's name.
 */
char const *plcrash_async_thread_state_get_reg_name (plcrash_async_thread_state_t *thread_state, plcrash_regnum_t regnum);

/**
 * Get the total number of registers supported by the @a cursor's target thread.
 *
 * @param cursor The target cursor.
 */
size_t plcrash_async_thread_state_get_reg_count (plcrash_async_thread_state_t *thread_state);

/**
 * Get a register value.
 */
plcrash_error_t plcrash_async_thread_state_get_reg (plcrash_async_thread_state_t *thread_state, plcrash_regnum_t regnum, plcrash_greg_t *reg);

/**
 * Set a register value.
 */
void plcrash_async_thread_state_set_reg (plcrash_async_thread_state_t *thread_state, plcrash_regnum_t regnum, plcrash_greg_t *reg);

/**
 * @}
 */

#endif /* PLCRASH_ASYNC_THREAD_H */