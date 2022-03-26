

void * __cdecl operator new(size_t size, POOL_TYPE type = PagedPool){
	DbgPrint(("new ����"));
	return ExAllocatePool(type, size);
}
void __cdecl operator delete(void*pointer){
	DbgPrint(("delete ����"));
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
		DbgPrint("�ڴ���ͬ");
	}
	else{
		DbgPrint("�ڴ治ͬ");
	}

	DbgPrint("���");
	RtlZeroBytes(s, length);
	ulRet = RtlCompareMemory(s, d, length);
	if (ulRet == length){
		DbgPrint("�ڴ���ͬ");
	}
	else{
		DbgPrint("�ڴ治ͬ");
	}

	ExFreePool(d);
	ExFreePool(s);
	DbgPrint("d=%x\n", (int*)d);
	return;
}
