
#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef	__cplusplus
}
#endif


#define INITCODE code_seg("INIT")
#define PAGECODE code_seg("PAGE")

#pragma PAGECODE

UNICODE_STRING symble;

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

void modifyMem(PJMPCODE des, PJMPCODE ori);

VOID DDK_Unload(IN PDRIVER_OBJECT pDriv){
	if (_bHook){
		modifyMem(_pCurCode, &_oldCode);
	}
	PDEVICE_OBJECT pObj = pDriv->DeviceObject;
	if (pObj != 0){
		IoDeleteDevice(pObj);
		IoDeleteSymbolicLink(&symble); //注意，必须删除符号连接，否则可能蓝屏
		DbgPrint(("删除设备对象成功"));
	}
	DbgPrint(("驱动unload成功"));
}

static const ULONG add_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS);

NTSTATUS
ddk_DispatchRoutine(
__in struct _DEVICE_OBJECT *DeviceObject,
__inout struct _IRP *Irp
){
	DbgPrint(("enter dispatch routine1"));
	//获取当前irp的栈
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG mf = stack->MajorFunction;
	switch (mf)
	{
	case IRP_MJ_CREATE:
		break;
	case IRP_MJ_READ:
		break;
	case IRP_MJ_WRITE:
		break;
	case IRP_MJ_DEVICE_CONTROL:
	{
		DbgPrint("enter device IO control");
		ULONG cbin = stack->Parameters.DeviceIoControl.InputBufferLength;
		DbgPrint("cbin: %d", cbin);
		ULONG cbout = stack->Parameters.DeviceIoControl.OutputBufferLength;
		DbgPrint("cbout: %d", cbout);

		PVOID inBuf = Irp->AssociatedIrp.SystemBuffer;
		DbgPrint("inbuf: %d, %d", *(int*)inBuf, *((int*)inBuf + 1));
		PVOID outBuf = Irp->AssociatedIrp.SystemBuffer;

		ULONG ioCode = stack->Parameters.DeviceIoControl.IoControlCode;

		switch (ioCode)
		{
		case add_code:
		{
			int a, b;
			DbgPrint("add_code");
			_asm{
				mov eax, inBuf
				mov ebx, [eax]
				mov a,ebx
				mov ebx, [eax+4]
				mov b,ebx
			}
			a = a + b;
			_asm{
				mov eax,a
				mov ebx, outBuf
				mov [ebx], eax
			}

			//*(int*)inBuf = 456;

		}
			break;
		default:
			break;
		}
	}
		
		break;
	case IRP_MJ_CLOSE:
		break;
	default:
		break;
	}

	Irp->IoStatus.Information = 1; //注意，这里返回的是实际outbuf的长度。不能乱填。否则缓冲区往上层拷贝时，会拷贝指定长度。
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


void * __cdecl operator new(size_t size, POOL_TYPE type = PagedPool){
	DbgPrint(("new 重载"));
	return ExAllocatePool(type, size);
}
void __cdecl operator delete(void*pointer){
	DbgPrint(("delete 重载"));
	ExFreePool(pointer);
}

#pragma INITCODE
VOID memtest(){
	VOID UNALIGNED *d;
	VOID UNALIGNED *s;
	SIZE_T length = 8;
	ULONG ulRet;
	char*buffer = new(PagedPool)char[100];
	delete buffer;

	s = ExAllocatePool(PagedPool, length);
	DbgPrint("s=%x\n", (int*)s);

	d = ExAllocatePool(PagedPool, length);
	DbgPrint("d=%x\n", (int*)d);

	RtlFillMemory(s, length, 's');
	RtlCopyMemory(d, s, length);

	ulRet = RtlCompareMemory(s, d, length);
	if (ulRet == length){
		DbgPrint("内存相同");
	}
	else{
		DbgPrint("内存不同");
	}

	DbgPrint("清空");
	RtlZeroBytes(s, length);
	ulRet = RtlCompareMemory(s, d, length);
	if (ulRet == length){
		DbgPrint("内存相同");
	}
	else{
		DbgPrint("内存不同");
	}

	ExFreePool(d);
	ExFreePool(s);
	DbgPrint("d=%x\n", (int*)d);
	return;
}

void listTest(){
	LIST_ENTRY listHead;
	InitializeListHead(&listHead);
	LIST_ENTRY *p1 = new(PagedPool)LIST_ENTRY[5];
	InsertHeadList(&listHead, &p1[0]);
	InsertTailList(&listHead, &p1[2]);

	delete[]p1;
}

void testCreateDev(PDRIVER_OBJECT p){
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\fx设备1");

	PDEVICE_OBJECT _pDevObj = 0;
	NTSTATUS ret = IoCreateDevice(p, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &_pDevObj);
	if (!NT_SUCCESS(ret)){
		DbgPrint("创建设备失败");
		return;
	}
	_pDevObj->Flags |= DO_BUFFERED_IO;

	
	RtlInitUnicodeString(&symble, L"\\??\\fx_dev1");
	ret = IoCreateSymbolicLink(&symble, &devName);
	if (!NT_SUCCESS(ret)){
		IoDeleteDevice(_pDevObj);
		return;
	}
	else{
		DbgPrint("创建设备成功");
	}
}

typedef struct _ServiceDescriptorTable{
	PVOID ServiceTableBase;
	PVOID ServiceCounterTable;
	unsigned int NumberOfServices;
	PVOID ParamTableBase;
}*PServiceDescriptorTable;

extern "C" PServiceDescriptorTable KeServiceDescriptorTable;

void getSSDT1(){
	ULONG SSDT_NtOpenProcess_Addr;
	__asm
	{
		push ebx
		push eax
		mov ebx,KeServiceDescriptorTable
		mov ebx, [ebx]
		mov eax, 0x7a
		shl eax, 2
		add ebx, eax
		mov ebx, [ebx]
		mov SSDT_NtOpenProcess_Addr,ebx
		pop eax
		pop ebx
	}
	DbgPrint("SSDT地址1: %x", SSDT_NtOpenProcess_Addr);
}

LONG getSSDT2(){
	LONG *SSDT_Adr, SSDT_NtOpenProcess_Cure_Addr, t_addr;
	t_addr = (LONG)KeServiceDescriptorTable->ServiceTableBase;
	SSDT_Adr = (PLONG)(t_addr + 0x7A * 4);
	SSDT_NtOpenProcess_Cure_Addr = *SSDT_Adr;

	DbgPrint("SSDT地址2: %x", SSDT_NtOpenProcess_Cure_Addr);
	return SSDT_NtOpenProcess_Cure_Addr;
}

LONG getSSDT3(){
	UNICODE_STRING s;
	LONG addr;
	RtlInitUnicodeString(&s, L"NtOpenProcess");
	addr = (LONG)MmGetSystemRoutineAddress(&s);
	DbgPrint("SSDT地址3: %x", addr);
	return addr;
}



void modifyMem(PJMPCODE des, PJMPCODE ori){
	__asm{
		cli
			mov eax, cr0
			and eax, 0xFFFEFFFF
			mov cr0, eax
			retn
	}

	des->E9 = ori->E9;
	des->JMPADDR = ori->JMPADDR; //跳转的相对地址

	__asm{
		mov eax, cr0
			or eax, 0x10000
			mov cr0, eax
			sti
			retn
	}
}
void testSSDT(){
	//getSSDT1();
	LONG curAddr = getSSDT2();
	LONG oriAddr = getSSDT3();
	if (curAddr != oriAddr){
		_bHook = true;
		_pCurCode = (PJMPCODE)curAddr;
		//记录
		_oldCode.E9 = ((PJMPCODE)curAddr)->E9;
		_oldCode.JMPADDR = ((PJMPCODE)curAddr)->JMPADDR;
		
		//还原原地址
		JMPCODE newCode;
		newCode.E9 = 0xE9;
		newCode.JMPADDR = curAddr - oriAddr - 5; //跳转的相对地址

		modifyMem(_pCurCode, &newCode);

	}
	else{
		DbgPrint("地址相同");
	}
}

#pragma INITCODE

#ifdef __cplusplus
extern "C"
#endif
NTSTATUS DriverEntry(PDRIVER_OBJECT p, PUNICODE_STRING s){
	DbgPrint(("驱动程序加载"));
	p->DriverUnload = DDK_Unload;

	p->MajorFunction[IRP_MJ_CREATE] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_WRITE] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_READ] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_CLOSE] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ddk_DispatchRoutine;

	testCreateDev(p);
	testSSDT();
	//memtest();
	//listTest();

	return 0;
}