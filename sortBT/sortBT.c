#include "Everything.h"

#define KEY_SIZE 8

typedef struct _TREENODE {
	struct _TREENODE* left, * right;
	TCHAR key[KEY_SIZE];
	LPTSTR pData;
} TREENODE, *LPTNODE, **LPPTNODE;

#define NODE_SIZE sizeof(TREENODE)
#define NODE_HEAP_ISIZE 0x8000
#define DATA_HEAP_ISIZE 0x8000
#define MAX_DATA_LEN 0x1000
#define TKEY_SIZE KEY_SIZE * sizeof(TCHAR)
#define STATUS_FILE_ERROR 0xE0000001

LPTNODE FillTree(HANDLE, HANDLE, HANDLE);
BOOL Scan(LPTNODE);
INT KeyCompare(LPCTSTR, LPCTSTR), iFile;
BOOL InsertTree(LPPTNODE, LPTNODE);

INT _tmain(INT argc, LPTSTR argv[]) {
	HANDLE hIn = INVALID_HANDLE_VALUE, hNodeHeap = NULL, hDataHeap = NULL;
	LPTNODE pRoot;
	TCHAR errorMessage[256];
	BOOL noPrint = FALSE;

	INT iFirstFile = Options(argc, argv, _T("n"), &noPrint, NULL);

	if (argc <= iFirstFile)
		ReportError(_T("Usage: sortBT [options] files"), 1, FALSE);

	for (iFile = iFirstFile; iFile < argc; iFile++) __try {
		hIn = CreateFile(argv[iFile], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hIn == INVALID_HANDLE_VALUE)
			RaiseException(STATUS_FILE_ERROR, 0, 0, NULL);
		__try {
			hNodeHeap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, NODE_HEAP_ISIZE, 0);
			hDataHeap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, DATA_HEAP_ISIZE, 0);

			pRoot = FillTree(hIn, hNodeHeap, hDataHeap);
			if (!noPrint) {
				_tprintf(_T("Sorted file: %s\n"), argv[iFile]);
				Scan(pRoot);
			}
		}
		__finally {
			if (hNodeHeap != NULL) {
				HeapDestroy(hNodeHeap);
				hNodeHeap = NULL;
			}
			if (hDataHeap != NULL) {
				HeapDestroy(hDataHeap);
				hDataHeap = NULL;
			}

			if (hIn != INVALID_HANDLE_VALUE) {
				CloseHandle(hIn);
				hIn = INVALID_HANDLE_VALUE;
			}
		}
	}
	__except ((GetExceptionCode() == STATUS_FILE_ERROR || GetExceptionCode() == STATUS_NO_MEMORY) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
		_stprintf_s(errorMessage, sizeof(errorMessage) / TSIZE, _T("\n%s %s"), _T("sortBT error on file:"), argv[iFile]);
		ReportError(errorMessage, 0, TRUE);
	}
	return EXIT_SUCCESS;
}

LPTNODE FillTree(HANDLE hIn, HANDLE hNode, HANDLE hData) {
	LPTNODE pRoot = NULL, pNode = NULL;
	DWORD nRead = 0, i;
	BOOL atCR;
	TCHAR dataHold[MAX_DATA_LEN];
	LPTSTR pString;

	while (TRUE) {
		pNode = HeapAlloc(hNode, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, NODE_SIZE);
		if (!ReadFile(hIn, pNode->key, TKEY_SIZE, &nRead, NULL) || nRead != TKEY_SIZE)
			return pRoot;

		atCR = FALSE;
		for (i = 0; i < MAX_DATA_LEN; i++) {
			ReadFile(hIn, &dataHold[i], TSIZE, &nRead, NULL);
			if (atCR && dataHold[i] == LF) break;
			atCR = (dataHold[i] == CR);
		}
		dataHold[i - 1] = _T('\0');

		SIZE_T szString = (SIZE_T)((KEY_SIZE + _tcslen(dataHold) + 1) * TSIZE);
		pString = HeapAlloc(hData, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, szString);
		memcpy_s(pString, szString, pNode->key, TKEY_SIZE);
		pString[KEY_SIZE] = _T('\0');
		_tcscat_s(pString, szString / TSIZE, dataHold);
		pNode->pData = pString;
		InsertTree(&pRoot, pNode);
	}
	return NULL;
}

BOOL InsertTree(LPPTNODE ppRoot, LPTNODE pNode) {
	if (*ppRoot == NULL) {
		*ppRoot = pNode;
		return TRUE;
	}
	if (KeyCompare(pNode->key, (*ppRoot)->key) < 0) {
		InsertTree(&((*ppRoot)->left), pNode);
	}
	else {
		InsertTree(&((*ppRoot)->right), pNode);
	}
}

static INT KeyCompare(LPCTSTR pKey1, LPCTSTR pKey2) {
	return _tcsncmp(pKey1, pKey2, KEY_SIZE);
}

static BOOL Scan(LPTNODE pNode) {
	if (pNode == NULL) return TRUE;
	Scan(pNode->left);
	_tprintf(_T("%s\n"), pNode->pData);
	Scan(pNode->right);
	return TRUE;
}