/*
 * error_handler.hxx
 * 
 * This header defines error handling that can be incoperated into an application
 * allowing for "last chance" information to be extracted following an handled 
 * exception or abort occuring. 
 * 
 * This code is adapted from an answer on StackOverflow (CC-BY-SA)
 * https://stackoverflow.com/a/2445569/1185
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <execinfo.h>
#include <signal.h>
#include <string.h>

#include <iostream>
#include <cstdlib>
#include <stdexcept>

// This structure mirrors the one found in /usr/include/asm/ucontext.h
typedef struct _sig_ucontext {
   unsigned long     uc_flags;
   struct ucontext   *uc_link;
   stack_t           uc_stack;
   struct sigcontext uc_mcontext;
   sigset_t          uc_sigmask;
} sig_ucontext_t;

// When the application aborts this method will be called prior to complete 
// termination allow for some information on the error to be extracted. This 
// should be treated as a last chance method since most of the information is 
// only of use to developers.
// 
// This function needs to be set for the sigaction function on the SIGABRT signal
// in order to operate.
void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext) {
    sig_ucontext_t * uc = (sig_ucontext_t *)ucontext;

    // Get the address at the time the signal was raised from the RIP (x86-64)
    void * caller_address = (void *) uc->uc_mcontext.rip;

    std::cerr << std::endl
              << "signal " << sig_num 
              << " (" << strsignal(sig_num) << "), address is " 
              << info->si_addr << " from " 
              << caller_address << std::endl;

    void * array[50];
    int size = backtrace(array, 50);

    std::cerr << __FUNCTION__ << " backtrace returned " 
              << size << " frames\n\n";

    // overwrite sigaction with caller's address
    array[1] = caller_address;

    char ** messages = backtrace_symbols(array, size);

    // skip first stack frame (points here)
    for (int i = 1; i < size && messages != NULL; ++i) {
        std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
    }
    std::cerr << std::endl;

    // Clean-up and terminate the program
    free(messages);
    exit(EXIT_FAILURE);
}

// When an unhandled exception occurs the method will be called prior to abort()
// being handled allowing for some information on the exception to be extracted. 
// This should be treated as a last chance method since most of the information 
// is only of use to developers.
// 
// This method needs to be registered with std::set_terminate in order to operate. 
void crit_err_terminate() {
    static bool tried_throw = false;

    // Attempt to rethrow the exception to see if we can learn more
    try {
        // try once to re-throw currently active exception
        if (!tried_throw) { 
          tried_throw = true;
          throw;
        }
    }
    catch (const std::exception &e) {
        std::cerr << __FUNCTION__ << " caught unhandled exception." << std::endl
                  << "type: " << typeid(e).name() << std::endl 
                  << "what(): " << e.what() << std::endl;
                  
    }
    catch (...) {
        std::cerr << __FUNCTION__ << " caught unknown/unhandled exception." << std::endl;
    }

    // Generate a backtrack on the error frames
    void * array[50];
    int size = backtrace(array, 50);    
    std::cerr << __FUNCTION__ << " backtrace returned " << size << " frames\n\n";
    char ** messages = backtrace_symbols(array, size);
    for (int i = 0; i < size && messages != NULL; ++i) {
        std::cerr << "[bt]: (" << i << ") " << messages[i] << std::endl;
    }
    std::cerr << std::endl;

    // Clean up and abort the program
    free(messages);
    abort();
}