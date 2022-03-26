#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef	__cplusplus
}
#endif

VOID testCreateFile(){
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objAttributes;
	IO_STATUS_BLOCK ioStatus;
	HANDLE hFile;
	UNICODE_STRING symbolFileName;

	RtlInitUnicodeString(&symbolFileName, L"\\Device\\HarddiskVolume1\\fxCreateFileTest.txt");
//	RtlInitUnicodeString(&symbolFileName, L"\\??\\c:\\fxCreateFileTest.txt");

	InitializeObjectAttributes(&objAttributes, &symbolFileName, OBJ_CASE_INSENSITIVE, 0, 0);

	ntStatus = ZwCreateFile(&hFile, GENERIC_WRITE, &objAttributes, &ioStatus, 0,
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);
	if (NT_SUCCESS(ntStatus)){
		DbgPrint("�����ļ��ɹ�\n");
	}
	else{
		DbgPrint("�����ļ�ʧ��\n");
	}
	ZwClose(hFile);
}

void testOpenFIle(){
	UNICODE_STRING symbolFileName;
	RtlInitUnicodeString(&symbolFileName, L"\\Device\\HarddiskVolume1\\fxCreateFileTest.txt");
	//	RtlInitUnicodeString(&symbolFileName, L"\\??\\c:\\fxCreateFileTest.txt");
	OBJECT_ATTRIBUTES objAttributes;
	InitializeObjectAttributes(&objAttributes, &symbolFileName, OBJ_CASE_INSENSITIVE, 0, 0);

	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus;
	NTSTATUS stat = ZwOpenFile(&hFile, GENERIC_ALL, &objAttributes, &ioStatus, FILE_SHARE_READ, FILE_OPEN_IF);
	if (NT_SUCCESS(stat)){
		DbgPrint("���ļ��ɹ�\n");
	}
	else{
		DbgPrint("���ļ�ʧ��\n");
	}
	ZwClose(hFile);
}

void testAttribute(){
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objAttributes;
	IO_STATUS_BLOCK ioStatus;
	HANDLE hFile;
	UNICODE_STRING symbolFileName;

//	RtlInitUnicodeString(&symbolFileName, L"\\Device\\HarddiskVolume1\\fxCreateFileTest.txt");
		RtlInitUnicodeString(&symbolFileName, L"\\??\\c:\\fxCreateFileTest.txt");

	InitializeObjectAttributes(&objAttributes, &symbolFileName, OBJ_CASE_INSENSITIVE, 0, 0);

	ntStatus = ZwCreateFile(&hFile, GENERIC_WRITE, &objAttributes, &ioStatus, 0,
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);
	if (NT_SUCCESS(ntStatus)){
		DbgPrint("�����ļ��ɹ�\n");
	}
	else{
		DbgPrint("�����ļ�ʧ��\n");
	}

	FILE_STANDARD_INFORMATION fInfo;
	ntStatus = ZwQueryInformationFile(
		hFile,
		&ioStatus,
		&fInfo,
		sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (NT_SUCCESS(ntStatus)){
		DbgPrint("�ļ����ȣ�%d\n", fInfo.EndOfFile.QuadPart);
	}

	FILE_BASIC_INFORMATION fBaseInfo;
	ntStatus = ZwQueryInformationFile(
		hFile,
		&ioStatus,
		&fBaseInfo,
		sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
	if (NT_SUCCESS(ntStatus)){
		//DbgPrint("�ļ����ȣ�%d\n", fBaseInfo.EndOfFile.QuadPart);
	}
	fBaseInfo.FileAttributes = FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN;
	ntStatus = ZwSetInformationFile(
		hFile,
		&ioStatus,
		&fBaseInfo,
		sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
	if (NT_SUCCESS(ntStatus)){
		DbgPrint("�����ļ�����ֻ�����ɹ�\n");
	}


	ZwClose(hFile);
}


void testWriteFile(){
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objAttributes;
	IO_STATUS_BLOCK ioStatus;
	HANDLE hFile;
	UNICODE_STRING symbolFileName;

	//	RtlInitUnicodeString(&symbolFileName, L"\\Device\\HarddiskVolume1\\fxCreateFileTest.txt");
	RtlInitUnicodeString(&symbolFileName, L"\\??\\c:\\fxCreateFileTest.txt");

	InitializeObjectAttributes(&objAttributes, &symbolFileName, OBJ_CASE_INSENSITIVE, 0, 0);

	ntStatus = ZwCreateFile(&hFile, GENERIC_WRITE, &objAttributes, &ioStatus, 0,
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);
	if (NT_SUCCESS(ntStatus)){
		DbgPrint("�����ļ��ɹ�\n");
	}
	else{
		DbgPrint("�����ļ�ʧ��\n");
	}
	const int BUFFER_SIZE = 30;
	PCHAR pBuffer = (PCHAR)ExAllocatePool(PagedPool, BUFFER_SIZE);
	RtlFillMemory(pBuffer, BUFFER_SIZE, 's');
	RtlCopyMemory(pBuffer, "xxx", sizeof("xxx"));

	LARGE_INTEGER offsetPos;
	offsetPos.QuadPart = 0i64;
	ZwWriteFile(hFile, 0, 0, 0, &ioStatus, pBuffer, BUFFER_SIZE, &offsetPos, 0);
	DbgPrint("д��%d�ֽ�", ioStatus.Information);

	offsetPos.QuadPart = 30i64;
	ZwWriteFile(hFile, 0, 0, 0, &ioStatus, pBuffer, BUFFER_SIZE, &offsetPos, 0);
	DbgPrint("д��%d�ֽ�", ioStatus.Information);

	ExFreePool(pBuffer);
	ZwClose(hFile);
}

void testReadFile(){
	NTSTATUS ntStatus;
	OBJECT_ATTRIBUTES objAttributes;
	IO_STATUS_BLOCK ioStatus;
	HANDLE hFile;
	UNICODE_STRING symbolFileName;

	//	RtlInitUnicodeString(&symbolFileName, L"\\Device\\HarddiskVolume1\\fxCreateFileTest.txt");
	RtlInitUnicodeString(&symbolFileName, L"\\??\\c:\\fxCreateFileTest.txt");

	InitializeObjectAttributes(&objAttributes, &symbolFileName, OBJ_CASE_INSENSITIVE, 0, 0);

	ntStatus = ZwCreateFile(&hFile, GENERIC_WRITE, &objAttributes, &ioStatus, 0,
		FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, 0, 0);
	if (NT_SUCCESS(ntStatus)){
		DbgPrint("�����ļ��ɹ�\n");
	}
	else{
		DbgPrint("�����ļ�ʧ��\n");
	}

	FILE_STANDARD_INFORMATION fStandInfo;
	ntStatus = ZwQueryInformationFile(hFile, &ioStatus, &fStandInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);

	PCHAR pBuffer = (PCHAR)ExAllocatePool(PagedPool, fStandInfo.EndOfFile.QuadPart);
	
	ZwReadFile(hFile, 0, 0, 0, &ioStatus, pBuffer, fStandInfo.EndOfFile.QuadPart,0,0);
	DbgPrint("��ȡ%d�ֽڣ�%s", ioStatus.Information, pBuffer);

	ExFreePool(pBuffer);
	ZwClose(hFile);
}