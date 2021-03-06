//===--- X86InstPrinterCommon.cpp - X86 assembly instruction printing -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file includes common code for rendering MCInst instances as Intel-style
// and Intel-style assembly.
//
//===----------------------------------------------------------------------===//

#include "X86InstPrinterCommon.h"
#include "MCTargetDesc/X86BaseInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include <cstdint>
#include <cassert>

using namespace llvm;

void X86InstPrinterCommon::printSSEAVXCC(const MCInst *MI, unsigned Op,
                                         raw_ostream &O) {
  int64_t Imm = MI->getOperand(Op).getImm();
  switch (Imm) {
  default: llvm_unreachable("Invalid ssecc/avxcc argument!");
  case    0: O << "eq"; break;
  case    1: O << "lt"; break;
  case    2: O << "le"; break;
  case    3: O << "unord"; break;
  case    4: O << "neq"; break;
  case    5: O << "nlt"; break;
  case    6: O << "nle"; break;
  case    7: O << "ord"; break;
  case    8: O << "eq_uq"; break;
  case    9: O << "nge"; break;
  case  0xa: O << "ngt"; break;
  case  0xb: O << "false"; break;
  case  0xc: O << "neq_oq"; break;
  case  0xd: O << "ge"; break;
  case  0xe: O << "gt"; break;
  case  0xf: O << "true"; break;
  case 0x10: O << "eq_os"; break;
  case 0x11: O << "lt_oq"; break;
  case 0x12: O << "le_oq"; break;
  case 0x13: O << "unord_s"; break;
  case 0x14: O << "neq_us"; break;
  case 0x15: O << "nlt_uq"; break;
  case 0x16: O << "nle_uq"; break;
  case 0x17: O << "ord_s"; break;
  case 0x18: O << "eq_us"; break;
  case 0x19: O << "nge_uq"; break;
  case 0x1a: O << "ngt_uq"; break;
  case 0x1b: O << "false_os"; break;
  case 0x1c: O << "neq_os"; break;
  case 0x1d: O << "ge_oq"; break;
  case 0x1e: O << "gt_oq"; break;
  case 0x1f: O << "true_us"; break;
  }
}

void X86InstPrinterCommon::printVPCOMMnemonic(const MCInst *MI,
                                              raw_ostream &OS) {
  OS << "vpcom";

  int64_t Imm = MI->getOperand(MI->getNumOperands() - 1).getImm();
  switch (Imm) {
  default: llvm_unreachable("Invalid vpcom argument!");
  case 0: OS << "lt"; break;
  case 1: OS << "le"; break;
  case 2: OS << "gt"; break;
  case 3: OS << "ge"; break;
  case 4: OS << "eq"; break;
  case 5: OS << "neq"; break;
  case 6: OS << "false"; break;
  case 7: OS << "true"; break;
  }

  switch (MI->getOpcode()) {
  default: llvm_unreachable("Unexpected opcode!");
  case X86::VPCOMBmi:  case X86::VPCOMBri:  OS << "b\t";  break;
  case X86::VPCOMDmi:  case X86::VPCOMDri:  OS << "d\t";  break;
  case X86::VPCOMQmi:  case X86::VPCOMQri:  OS << "q\t";  break;
  case X86::VPCOMUBmi: case X86::VPCOMUBri: OS << "ub\t"; break;
  case X86::VPCOMUDmi: case X86::VPCOMUDri: OS << "ud\t"; break;
  case X86::VPCOMUQmi: case X86::VPCOMUQri: OS << "uq\t"; break;
  case X86::VPCOMUWmi: case X86::VPCOMUWri: OS << "uw\t"; break;
  case X86::VPCOMWmi:  case X86::VPCOMWri:  OS << "w\t";  break;
  }
}

void X86InstPrinterCommon::printVPCMPMnemonic(const MCInst *MI,
                                              raw_ostream &OS) {
  OS << "vpcmp";

  printSSEAVXCC(MI, MI->getNumOperands() - 1, OS);

  switch (MI->getOpcode()) {
  default: llvm_unreachable("Unexpected opcode!");
  case X86::VPCMPBZ128rmi:  case X86::VPCMPBZ128rri:
  case X86::VPCMPBZ256rmi:  case X86::VPCMPBZ256rri:
  case X86::VPCMPBZrmi:     case X86::VPCMPBZrri:
  case X86::VPCMPBZ128rmik: case X86::VPCMPBZ128rrik:
  case X86::VPCMPBZ256rmik: case X86::VPCMPBZ256rrik:
  case X86::VPCMPBZrmik:    case X86::VPCMPBZrrik:
    OS << "b\t";
    break;
  case X86::VPCMPDZ128rmi:  case X86::VPCMPDZ128rri:
  case X86::VPCMPDZ256rmi:  case X86::VPCMPDZ256rri:
  case X86::VPCMPDZrmi:     case X86::VPCMPDZrri:
  case X86::VPCMPDZ128rmik: case X86::VPCMPDZ128rrik:
  case X86::VPCMPDZ256rmik: case X86::VPCMPDZ256rrik:
  case X86::VPCMPDZrmik:    case X86::VPCMPDZrrik:
  case X86::VPCMPDZ128rmib: case X86::VPCMPDZ128rmibk:
  case X86::VPCMPDZ256rmib: case X86::VPCMPDZ256rmibk:
  case X86::VPCMPDZrmib:    case X86::VPCMPDZrmibk:
    OS << "d\t";
    break;
  case X86::VPCMPQZ128rmi:  case X86::VPCMPQZ128rri:
  case X86::VPCMPQZ256rmi:  case X86::VPCMPQZ256rri:
  case X86::VPCMPQZrmi:     case X86::VPCMPQZrri:
  case X86::VPCMPQZ128rmik: case X86::VPCMPQZ128rrik:
  case X86::VPCMPQZ256rmik: case X86::VPCMPQZ256rrik:
  case X86::VPCMPQZrmik:    case X86::VPCMPQZrrik:
  case X86::VPCMPQZ128rmib: case X86::VPCMPQZ128rmibk:
  case X86::VPCMPQZ256rmib: case X86::VPCMPQZ256rmibk:
  case X86::VPCMPQZrmib:    case X86::VPCMPQZrmibk:
    OS << "q\t";
    break;
  case X86::VPCMPUBZ128rmi:  case X86::VPCMPUBZ128rri:
  case X86::VPCMPUBZ256rmi:  case X86::VPCMPUBZ256rri:
  case X86::VPCMPUBZrmi:     case X86::VPCMPUBZrri:
  case X86::VPCMPUBZ128rmik: case X86::VPCMPUBZ128rrik:
  case X86::VPCMPUBZ256rmik: case X86::VPCMPUBZ256rrik:
  case X86::VPCMPUBZrmik:    case X86::VPCMPUBZrrik:
    OS << "ub\t";
    break;
  case X86::VPCMPUDZ128rmi:  case X86::VPCMPUDZ128rri:
  case X86::VPCMPUDZ256rmi:  case X86::VPCMPUDZ256rri:
  case X86::VPCMPUDZrmi:     case X86::VPCMPUDZrri:
  case X86::VPCMPUDZ128rmik: case X86::VPCMPUDZ128rrik:
  case X86::VPCMPUDZ256rmik: case X86::VPCMPUDZ256rrik:
  case X86::VPCMPUDZrmik:    case X86::VPCMPUDZrrik:
  case X86::VPCMPUDZ128rmib: case X86::VPCMPUDZ128rmibk:
  case X86::VPCMPUDZ256rmib: case X86::VPCMPUDZ256rmibk:
  case X86::VPCMPUDZrmib:    case X86::VPCMPUDZrmibk:
    OS << "ud\t";
    break;
  case X86::VPCMPUQZ128rmi:  case X86::VPCMPUQZ128rri:
  case X86::VPCMPUQZ256rmi:  case X86::VPCMPUQZ256rri:
  case X86::VPCMPUQZrmi:     case X86::VPCMPUQZrri:
  case X86::VPCMPUQZ128rmik: case X86::VPCMPUQZ128rrik:
  case X86::VPCMPUQZ256rmik: case X86::VPCMPUQZ256rrik:
  case X86::VPCMPUQZrmik:    case X86::VPCMPUQZrrik:
  case X86::VPCMPUQZ128rmib: case X86::VPCMPUQZ128rmibk:
  case X86::VPCMPUQZ256rmib: case X86::VPCMPUQZ256rmibk:
  case X86::VPCMPUQZrmib:    case X86::VPCMPUQZrmibk:
    OS << "uq\t";
    break;
  case X86::VPCMPUWZ128rmi:  case X86::VPCMPUWZ128rri:
  case X86::VPCMPUWZ256rri:  case X86::VPCMPUWZ256rmi:
  case X86::VPCMPUWZrmi:     case X86::VPCMPUWZrri:
  case X86::VPCMPUWZ128rmik: case X86::VPCMPUWZ128rrik:
  case X86::VPCMPUWZ256rrik: case X86::VPCMPUWZ256rmik:
  case X86::VPCMPUWZrmik:    case X86::VPCMPUWZrrik:
    OS << "uw\t";
    break;
  case X86::VPCMPWZ128rmi:  case X86::VPCMPWZ128rri:
  case X86::VPCMPWZ256rmi:  case X86::VPCMPWZ256rri:
  case X86::VPCMPWZrmi:     case X86::VPCMPWZrri:
  case X86::VPCMPWZ128rmik: case X86::VPCMPWZ128rrik:
  case X86::VPCMPWZ256rmik: case X86::VPCMPWZ256rrik:
  case X86::VPCMPWZrmik:    case X86::VPCMPWZrrik:
    OS << "w\t";
    break;
  }
}

void X86InstPrinterCommon::printRoundingControl(const MCInst *MI, unsigned Op,
                                                raw_ostream &O) {
  int64_t Imm = MI->getOperand(Op).getImm();
  switch (Imm) {
  default:
    llvm_unreachable("Invalid rounding control!");
  case X86::TO_NEAREST_INT:
    O << "{rn-sae}";
    break;
  case X86::TO_NEG_INF:
    O << "{rd-sae}";
    break;
  case X86::TO_POS_INF:
    O << "{ru-sae}";
    break;
  case X86::TO_ZERO:
    O << "{rz-sae}";
    break;
  }
}

/// printPCRelImm - This is used to print an immediate value that ends up
/// being encoded as a pc-relative value (e.g. for jumps and calls).  In
/// Intel-style these print slightly differently than normal immediates.
/// for example, a $ is not emitted.
void X86InstPrinterCommon::printPCRelImm(const MCInst *MI, unsigned OpNo,
                                         raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm())
    O << formatImm(Op.getImm());
  else {
    assert(Op.isExpr() && "unknown pcrel immediate operand");
    // If a symbolic branch target was added as a constant expression then print
    // that address in hex.
    const MCConstantExpr *BranchTarget = dyn_cast<MCConstantExpr>(Op.getExpr());
    int64_t Address;
    if (BranchTarget && BranchTarget->evaluateAsAbsolute(Address)) {
      O << formatHex((uint64_t)Address);
    } else {
      // Otherwise, just print the expression.
      Op.getExpr()->print(O, &MAI);
    }
  }
}

void X86InstPrinterCommon::printOptionalSegReg(const MCInst *MI, unsigned OpNo,
                                               raw_ostream &O) {
  if (MI->getOperand(OpNo).getReg()) {
    printOperand(MI, OpNo, O);
    O << ':';
  }
}

void X86InstPrinterCommon::printInstFlags(const MCInst *MI, raw_ostream &O) {
  const MCInstrDesc &Desc = MII.get(MI->getOpcode());
  uint64_t TSFlags = Desc.TSFlags;
  unsigned Flags = MI->getFlags();

  if ((TSFlags & X86II::LOCK) || (Flags & X86::IP_HAS_LOCK))
    O << "\tlock\t";

  if ((TSFlags & X86II::NOTRACK) || (Flags & X86::IP_HAS_NOTRACK))
    O << "\tnotrack\t";

  if (Flags & X86::IP_HAS_REPEAT_NE)
    O << "\trepne\t";
  else if (Flags & X86::IP_HAS_REPEAT)
    O << "\trep\t";
}
