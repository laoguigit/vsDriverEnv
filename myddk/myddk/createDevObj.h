
void testCreateDev(PDRIVER_OBJECT p){
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\fx�豸1");

	PDEVICE_OBJECT _pDevObj = 0;
	NTSTATUS ret = IoCreateDevice(p, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &_pDevObj);
	if (!NT_SUCCESS(ret)){
		DbgPrint("�����豸ʧ��");
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
		DbgPrint("�����豸�ɹ�");
	}
}