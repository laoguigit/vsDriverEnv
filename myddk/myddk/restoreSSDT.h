
#include "serviceDescriptorTable.h"

#pragma pack(push)
#pragma pack(1)
typedef struct{
	char E9;
	int JMPADDR;
}JMPCODE, *PJMPCODE;
#pragma pack(pop)

JMPCODE *_pCurCode = 0;
JMPCODE _oldCode;
bool _bHook = false;

#include "modifyMem.h"


void getSSDT1(){
	ULONG SSDT_NtOpenProcess_Addr;
	__asm
	{
		push ebx
			push eax
			mov ebx, KeServiceDescriptorTable
			mov ebx, [ebx]
			mov eax, 0x7a
			shl eax, 2
			add ebx, eax
			mov ebx, [ebx]
			mov SSDT_NtOpenProcess_Addr, ebx
			pop eax
			pop ebx
	}
	DbgPrint("SSDT��ַ1: %x", SSDT_NtOpenProcess_Addr);
}

LONG getSSDT2(){
	LONG *SSDT_Adr, SSDT_NtOpenProcess_Cure_Addr, t_addr;
	t_addr = (LONG)KeServiceDescriptorTable->ServiceTableBase;
	SSDT_Adr = (PLONG)(t_addr + 0x7A * 4);
	SSDT_NtOpenProcess_Cure_Addr = *SSDT_Adr;

	DbgPrint("SSDT��ַ2: %x", SSDT_NtOpenProcess_Cure_Addr);
	return SSDT_NtOpenProcess_Cure_Addr;
}

LONG getSSDT3(){
	UNICODE_STRING s;
	LONG addr;
	RtlInitUnicodeString(&s, L"NtOpenProcess");
	addr = (LONG)MmGetSystemRoutineAddress(&s);
	DbgPrint("SSDT��ַ3: %x", addr);
	return addr;
}


#include "modifyMem.h"

void restoreSSDT(){
	//getSSDT1();
	LONG curAddr = getSSDT2();
	LONG oriAddr = getSSDT3();
	if (curAddr != oriAddr){
		_bHook = true;
		_pCurCode = (PJMPCODE)curAddr;
		//��¼
		_oldCode.E9 = ((PJMPCODE)curAddr)->E9;
		_oldCode.JMPADDR = ((PJMPCODE)curAddr)->JMPADDR;

		//��ԭԭ��ַ
		JMPCODE newCode;
		newCode.E9 = 0xE9;
		newCode.JMPADDR = curAddr - oriAddr - 5; //��ת����Ե�ַ

		modifyMem(_pCurCode, &newCode, sizeof(newCode));

	}
	else{
		DbgPrint("��ַ��ͬ");
	}
}
