void listTest(){
	LIST_ENTRY listHead;
	InitializeListHead(&listHead);
	LIST_ENTRY *p1 = new(PagedPool)LIST_ENTRY[5];
	InsertHeadList(&listHead, &p1[0]);
	InsertTailList(&listHead, &p1[2]);

	delete[]p1;
}
