
extern "C"{
	#include <ntddk.h>
}
#include "serviceDescriptorTable.h"

bool _ssdtHookFlag = false;
ULONG _realNtOpenProcessAddress;
HANDLE _myProtectPid; //要保护的进程handle，禁止打开

extern "C" typedef NTSTATUS __stdcall NTOPENPROCESS(
	PHANDLE procHandle,
	ACCESS_MASK desireAccess,
	POBJECT_ATTRIBUTES objAttr,
	PCLIENT_ID clientId);
NTOPENPROCESS *_realNtOpenProcess;

#define PAGECOD code_seg("PAGE")

#pragma PAGECODE
extern "C" NTSTATUS __stdcall MyNtOpenProcess(
	PHANDLE procHandle,
	ACCESS_MASK desireAccess,
	POBJECT_ATTRIBUTES objAttr,
	PCLIENT_ID clientId){
	NTSTATUS rc;
	rc = (NTSTATUS)_realNtOpenProcess(procHandle, desireAccess, objAttr, clientId);
	if (clientId == 0){
		return rc;
	}

	if (clientId->UniqueProcess == _myProtectPid){  //如果是指定id，拒绝访问。那之前realOpen是否多余？
		procHandle = 0;
		rc = STATUS_ACCESS_DENIED;
	}
	return rc;
}

#include "modifyMem.h"

#pragma PAGECODE
VOID hookSSDt(VOID* pid){
	if (_ssdtHookFlag){
		return;
	}
	DbgPrint("enter hook ssdt");

	_myProtectPid = *(HANDLE*)pid;

	_ssdtHookFlag = true;
	ULONG*ssdtAddr;
	ssdtAddr = (PULONG)((ULONG)(KeServiceDescriptorTable->ServiceTableBase) + 0x7A * 4);
	_realNtOpenProcessAddress = *ssdtAddr;
	_realNtOpenProcess = (NTOPENPROCESS*)_realNtOpenProcessAddress;

	DbgPrint("真实openprocess: %x\n", _realNtOpenProcessAddress);
	DbgPrint("伪造openprocess: %x\n", MyNtOpenProcess);

	/*__asm{
		cli
			mov eax, cr0
			and eax, not 10000h
			mov cr0,eax
	}
	*ssdtAddr = (ULONG)MyNtOpenProcess;
	__asm{
		mov eax, cr0
			or eax, 10000h
			mov cr0, eax
			sti
	}*/
	ULONG myAddr = (ULONG)MyNtOpenProcess;

	DbgPrint("modify 1\n");
	modifyMem(ssdtAddr, &myAddr, sizeof(myAddr));
	DbgPrint("modify 2\n");
	return;
}

void unhookSSDT(){
	PULONG old_ssdt;
	old_ssdt = (PULONG)((ULONG)(KeServiceDescriptorTable->ServiceTableBase) + 0x7A * 4);
	if (_ssdtHookFlag){
		_ssdtHookFlag = false;
		DbgPrint("还原openprocess: %x\n", _realNtOpenProcessAddress);
		modifyMem(old_ssdt, &_realNtOpenProcessAddress, sizeof(_realNtOpenProcessAddress));
		DbgPrint("还原openprocess: %x\n", _realNtOpenProcessAddress);
	}
}