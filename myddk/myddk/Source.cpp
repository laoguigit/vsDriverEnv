
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
VOID DDK_Unload(IN PDRIVER_OBJECT o){
	KdPrint(("ж�سɹ�"));
}


#pragma INITCODE

#ifdef __cplusplus
extern "C" 
#endif
NTSTATUS DriverEntry(PDRIVER_OBJECT p, PUNICODE_STRING s){
	KdPrint(("��һ����������"));
	p->DriverUnload = DDK_Unload;
	return 0;

}