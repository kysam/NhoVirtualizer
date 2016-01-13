#include "Translator.h"
#include "asm/udis86.h"
#include "asm/libudis86/decode.h"


Translator::Translator()
{
}


Translator::~Translator()
{
}

void Translator::Translate(uchar* native, int nativeSize, std::vector<NhoInstr>* nhos)
{
	int c = 0;
	ud_t dis;
	ud_init(&dis);
	ud_set_mode(&dis, 32);
	ud_set_syntax(&dis, UD_SYN_INTEL);
	NhoInstr nho;

	while (c < nativeSize) {
		ud_set_input_buffer(&dis, &native[c], nativeSize - c >= MAX_INSN_LENGTH ? MAX_INSN_LENGTH : nativeSize - c);
		c += ud_disassemble(&dis);
		nho.mnemonic = dis.mnemonic ^ MNE_XOR;
		memcpy(&nho.operands, &dis.operand, sizeof(nho.operands));

		nho.pfx_adr = dis.pfx_adr;
		nho.pfx_lock = dis.pfx_lock;
		nho.pfx_opr = dis.pfx_opr;
		nho.pfx_rep = dis.pfx_rep;
		nho.pfx_repe = dis.pfx_repe;
		nho.pfx_repne = dis.pfx_repne;
		nho.pfx_rex = dis.pfx_rex;
		nho.pfx_seg = dis.pfx_seg;
		nho.pfx_str = dis.pfx_str;
	}
}
