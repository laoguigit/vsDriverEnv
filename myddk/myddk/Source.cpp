
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

#include "restoreSSDT.h"
#include "testMem.h"
#include "dispatch.h"
#include "createDevObj.h"
#include "fileIO.h"

VOID DDK_Unload(IN PDRIVER_OBJECT pDriv){
	if (_bHook){
		modifyMem(_pCurCode, &_oldCode, sizeof(_oldCode));
	}
	PDEVICE_OBJECT pObj = pDriv->DeviceObject;
	if (pObj != 0){
		IoDeleteDevice(pObj);
		IoDeleteSymbolicLink(&symble); //ע�⣬����ɾ���������ӣ������������
		DbgPrint(("ɾ���豸����ɹ�"));
	}
	DbgPrint(("����unload�ɹ�"));
}



#pragma INITCODE

#ifdef __cplusplus
extern "C"
#endif
NTSTATUS DriverEntry(PDRIVER_OBJECT p, PUNICODE_STRING s){
	DbgPrint(("�����������"));
	p->DriverUnload = DDK_Unload;

	p->MajorFunction[IRP_MJ_CREATE] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_WRITE] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_READ] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_CLOSE] = ddk_DispatchRoutine;
	p->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ddk_DispatchRoutine;

	testCreateDev(p);
	testCreateFile();
	//testAttribute();
	testReadFile();
	//restoreSSDT();
	//memtest();
	//listTest();

	return 0;
}