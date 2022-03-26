
static const ULONG add_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS);
static const ULONG hook_ssdt_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS);
static const ULONG unhook_ssdt_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS);

#include "hookSSDT.h"

NTSTATUS  ioContro(struct _IRP *Irp){
	DbgPrint("enter device IO control");
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
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
				mov a, ebx
				mov ebx, [eax + 4]
				mov b, ebx
		}
		a = a + b;
		_asm{
			mov eax, a
				mov ebx, outBuf
				mov[ebx], eax
		}

		//*(int*)inBuf = 456;

	}
	break;
	case hook_ssdt_code:
		hookSSDt(inBuf);
		break;
	case unhook_ssdt_code:
		unhookSSDT();
		break;
	default:
		break;
	}
}

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
		ioContro(Irp);
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
