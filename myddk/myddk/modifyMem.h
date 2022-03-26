#pragma once


void modifyMem(VOID* des, VOID* ori, int len){
	__asm{
		cli
			push eax
			mov eax, cr0
			and eax, 0xFFFEFFFF
			mov cr0, eax
	}

	RtlCopyMemory(des, ori, len);
	
	__asm{
		mov eax, cr0
			or eax, 0x10000
			mov cr0, eax
			sti
			pop eax
	}
}