#pragma once
#include "stdafx.h"

// Header
// =============

typedef std::vector<std::pair<int, std::string>> procs;
typedef std::pair<int, std::string> proc;

namespace prochelper {
	procs getProcs(bool onlyDekaronProcs = false);
	uintptr_t getModuleBaseAddress(DWORD procId, const wchar_t* modName);
	uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
	HWND findWindowFromProcessId(DWORD dwProcessId);
}

// Body
// =============

inline procs prochelper::getProcs(bool onlyDekaronProcs) {
	procs result = procs();
	unsigned int total = 0;
	
	// Get the list of process identifiers.
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		return result;
	}
	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	// Iterate through the process list
	int listIndex = 0;
	for (i = 0; i < cProcesses; i++) {
		if (aProcesses[i] != 0)
		{
			// Process Name declaration
			WCHAR procName[100] = L"<unknown>";
			// Get a handle to the process.
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
			// Get the process name.
			if (NULL != hProcess) {
				HMODULE hMod;
				DWORD cbNeeded;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
					GetModuleBaseNameW(hProcess, hMod, procName, sizeof(procName) / sizeof(WCHAR));
				}
			}
			// Release the handle to the process.
			CloseHandle(hProcess);

			// If only dekaron procs requested
			if (onlyDekaronProcs && wcscmp(procName, L"zdkrc.exe") != 0) continue;

			// conv wchar to wstring
			std::wstring ws(procName);
			// conv wstring to string
			std::string strProcName(ws.begin(), ws.end());

			// Add the process to the result
			result.push_back(proc(aProcesses[i], strProcName));
			// Counting total process
			listIndex += 1;
		}
	}

	return result;
};

inline uintptr_t prochelper::getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (int)procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp((wchar_t*)modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

inline uintptr_t prochelper::findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}


static struct EnumData {
	DWORD dwProcessId;
	HWND hWnd;
};

// Application-defined callback for EnumWindows
inline static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam)
{
	// Retrieve storage location for communication data
	EnumData& ed = *(EnumData*)lParam;
	DWORD dwProcessId = 0x0;
	// Query process ID for hWnd
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	// Apply filter - if you want to implement additional restrictions,
	// this is the place to do so.
	if (ed.dwProcessId == dwProcessId)
	{
		// Found a window matching the process ID
		ed.hWnd = hWnd;
		// Report success
		SetLastError(ERROR_SUCCESS);
		// Stop enumeration
		return FALSE;
	}
	// Continue enumeration
	return TRUE;
}

inline HWND prochelper::findWindowFromProcessId(DWORD dwProcessId)
{
	EnumData ed = { dwProcessId };
	if (!EnumWindows(EnumProc, (LPARAM)&ed) &&
		(GetLastError() == ERROR_SUCCESS))
	{
		return ed.hWnd;
	}
	return NULL;
}
