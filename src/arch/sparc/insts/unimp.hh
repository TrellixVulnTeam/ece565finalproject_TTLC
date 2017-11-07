/*
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Steve Reinhardt
 */

#ifndef __ARCH_SPARC_INSTS_UNIMP_HH__
#define __ARCH_SPARC_INSTS_UNIMP_HH__

#include "arch/sparc/insts/static_inst.hh"
#include "base/cprintf.hh"

namespace SparcISA
{

////////////////////////////////////////////////////////////////////
//
// Unimplemented instructions
//

/**
 * Static instruction class for unimplemented instructions that
 * cause simulator termination.  Note that these are recognized
 * (legal) instructions that the simulator does not support; the
 * 'Unknown' class is used for unrecognized/illegal instructions.
 * This is a leaf class.
 */
class FailUnimplemented : public SparcStaticInst
{
  public:
    /// Constructor
    FailUnimplemented(const char *_mnemonic, ExtMachInst _machInst) :
            SparcStaticInst(_mnemonic, _machInst, No_OpClass)
    {
        // don't call execute() (which panics) if we're on a
        // speculative path
        flags[IsNonSpeculative] = true;
    }

    Fault
    execute(ExecContext *xc, Trace::InstRecord *traceData) const override
    {
        panic("attempt to execute unimplemented instruction '%s' "
              "(inst 0x%08x)", mnemonic, machInst);
    }

    std::string
    generateDisassembly(Addr pc, const SymbolTable *symtab) const override
    {
        return csprintf("%-10s (unimplemented)", mnemonic);
    }
};

/**
 * Base class for unimplemented instructions that cause a warning
 * to be printed (but do not terminate simulation).  This
 * implementation is a little screwy in that it will print a
 * warning for each instance of a particular unimplemented machine
 * instruction, not just for each unimplemented opcode.  Should
 * probably make the 'warned' flag a static member of the derived
 * class.
 */
class WarnUnimplemented : public SparcStaticInst
{
  private:
    /// Have we warned on this instruction yet?
    mutable bool warned;

  public:
    /// Constructor
    WarnUnimplemented(const char *_mnemonic, ExtMachInst _machInst) :
            SparcStaticInst(_mnemonic, _machInst, No_OpClass), warned(false)
    {
        // don't call execute() (which panics) if we're on a
        // speculative path
        flags[IsNonSpeculative] = true;
    }

    Fault
    execute(ExecContext *xc, Trace::InstRecord *traceData) const override
    {
        if (!warned) {
            warn("instruction '%s' unimplemented\n", mnemonic);
            warned = true;
        }
        return NoFault;
    }

    std::string
    generateDisassembly(Addr pc, const SymbolTable *symtab) const override
    {
        return csprintf("%-10s (unimplemented)", mnemonic);
    }
};

}

#endif // __ARCH_SPARC_INSTS_UNIMP_HH__
