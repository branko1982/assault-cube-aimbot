#include <windows.h>
#include <TlHelp32.h>
#include <string>

#include <iostream>
#include <experimental/filesystem>


DWORD get_process_id_by_name(std::string process_name)
{
	PROCESSENTRY32 processEntry;
	//DWORD priorityClass;
	HANDLE processSnapshotHadle = NULL;
	processSnapshotHadle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (processSnapshotHadle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else {
		processEntry.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(processSnapshotHadle, &processEntry))
		{
			CloseHandle(processSnapshotHadle);
		}

		// Now walk the snapshot of processes, and
		// display information about each process in turn
		do
		{
			if (strcmp(processEntry.szExeFile, process_name.c_str()) == 0) {
				return processEntry.th32ProcessID;
			}

		} while (Process32Next(processSnapshotHadle, &processEntry));

		CloseHandle(processSnapshotHadle);
	}
	return 0;
}



int main(int argc, char* argv[]) {

	std::string pathToDll = "C:\\Users\\user\\Desktop\\fun\\AC-aimbot\\Release\\Assault Cube.dll";
	std::string process_name = "ac_client.exe";
	DWORD process_id = 0;


	process_id = get_process_id_by_name(process_name);

	if (process_id == 0) {
		std::cout << "[!] failed getting a process id" << std::endl;

		return EXIT_SUCCESS;
	}
	bool processFound = false;

	PROCESSENTRY32 processEntry32 = { sizeof(PROCESSENTRY32) };
	HANDLE processSnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	while (Process32Next(processSnapHandle, &processEntry32) == 1) {
		if (process_id == processEntry32.th32ProcessID) {
			processFound = true;
			break;
		}
	}

	if (processFound) {

		if (std::experimental::filesystem::v1::exists(pathToDll) && std::experimental::filesystem::v1::is_regular_file(pathToDll)) {

			HANDLE targetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, process_id);
			if (targetProcessHandle) {


				LPVOID retrievedLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
				LPVOID targetMemoryLocation = VirtualAllocEx(targetProcessHandle, 0, strlen(pathToDll.c_str()), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

				if (targetMemoryLocation != 0) {

					WriteProcessMemory(targetProcessHandle, targetMemoryLocation, pathToDll.c_str(), strlen(pathToDll.c_str()), 0);

					HANDLE RemoteThread = CreateRemoteThread(targetProcessHandle, 0, 0, (LPTHREAD_START_ROUTINE)retrievedLibraryAddress, targetMemoryLocation, 0, 0);


					WaitForSingleObject(RemoteThread, INFINITE);
					VirtualFreeEx(targetProcessHandle, targetMemoryLocation, strlen(pathToDll.c_str()), MEM_RELEASE);
					CloseHandle(RemoteThread);
					CloseHandle(targetProcessHandle);


					std::cout << "[s] dll injected into process " << std::endl;
				}
				else {
					std::cout << "[!] failed allocating memory in target process for dll path" << std::endl;
				}
			}
		}
		else {
			std::cout << "[!] file does not exist : " << std::endl;

		}
	}
	else {
		std::cout << "[!] process not found." << std::endl;
	}
}