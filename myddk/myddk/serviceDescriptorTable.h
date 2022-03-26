#pragma once

typedef struct _ServiceDescriptorTable{
	PVOID ServiceTableBase;
	PVOID ServiceCounterTable;
	unsigned int NumberOfServices;
	PVOID ParamTableBase;
}*PServiceDescriptorTable;


extern "C" PServiceDescriptorTable KeServiceDescriptorTable;