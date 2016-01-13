#include "common/common.h"
#include "asm/libudis86/types.h"

#define MNE_XOR 0x46

struct NhoInstr {
	int mnemonic;
	ud_operand operands[3];
	uchar pfx_rex;
	uchar pfx_seg;
	uchar pfx_opr;
	uchar pfx_adr;
	uchar pfx_lock;
	uchar pfx_str;
	uchar pfx_rep;
	uchar pfx_repe;
	uchar pfx_repne;
};