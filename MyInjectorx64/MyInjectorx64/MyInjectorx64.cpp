#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <Windows.h>
#include <TlHelp32.h>
using namespace std;

void* TitleGangi(void*)
{
	string title = "";
	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution<int> distribution(0, 75);
	while (true)
	{
		title.clear();
		for (int i = 0; i < 40; i++)
		{
			char c = '0' + distribution(mt);         
			title += c;
		}
		SetConsoleTitleA(title.c_str());
		Sleep(50);
	}

}
void Gangi()
{
	cout << "               _      _         _   _  _____           \n";
	cout << "              | |    | |  /\\   | \\ | |/ ____|         \n";
	cout << "              | |    | | /  \\  |  \\| | |  __           \n";
	cout << "          _   | |_   | |/ /\\ \\ | . ` | | |_ |          \n";
	cout << "         | |__| | |__| / ____ \\| |\\  | |__| |          \n";
	cout << "        _ \\____/ \\____/_/____\\_\\_|_\\_|\\_____|__        \n";
	cout << "       | |  | |   /\\   / ____| |/ /  ____|  __ \\       \n";
	cout << "       | |__| |  /  \\ | |    | ' /| |__  | |__) |      \n";
	cout << "       |  __  | / /\\ \\| |    |  < |  __| |  _  /   \n";
	cout << "       | |  | |/ ____ \\ |____| . \\| |____| | \\ \\       \n";
	cout << "  _____|_|  |_/_/  _ \\_\\_____|_|\\_\\______|_|__\\_\\____  \n";
	cout << " |_   _| \\ | |    | |  ____/ ____|__   __/ __ \\|  __ \\ \n";
	cout << "   | | |  \\| |    | | |__ | |       | | | |  | | |__) |\n";
	cout << "   | | | . ` |_   | |  __|| |       | | | |  | |  _  / \n";
	cout << "  _| |_| |\\  | |__| | |___| |____   | | | |__| | | \\ \\ \n";
	cout << " |_____|_| \\_|\\____/|______\\_____|  |_|  \\____/|_|  \\_\\\n";

}

HANDLE GetProcByName(wstring Proc)
{
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(pe);
	auto dwPid = -1;
	auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(hSnap, &pe))
	{
		do
		{
			if (_wcsicmp(Proc.c_str(), pe.szExeFile) == 0)
			{
				dwPid = pe.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnap, &pe));
	}
	return  OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPid);
}



bool Inject(wstring Proc, wstring Dll)
{
	DWORD dwModule;
	wchar_t szPath[MAX_PATH] = { 0, };
	auto hProc = GetProcByName(Proc);
	auto Params = VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	GetCurrentDirectory(sizeof(szPath), szPath);
	wcscat_s(szPath, (L"\\" + Dll).c_str());
	WriteProcessMemory(hProc, Params, (void*)szPath, sizeof(szPath) * 2, NULL);
	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, Params, 0, NULL);
	VirtualFreeEx(hProc, Params, 0, MEM_FREE);
	if (hProc)
		CloseHandle(hProc);
	if (hThread)
		CloseHandle(hThread);
	return reinterpret_cast<bool>(hThread);
}

int wmain(int argc, wchar_t* argv[])
{
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)TitleGangi, NULL, NULL, NULL);
	system("color 0a");
	system("MODE CON COLS=60 LINES=19");
	Gangi();
	wstring proc, dll;
	wfstream file("config.dat");
	if (file.is_open())
	{
		getline(file, proc, L'|');
		getline(file, dll);
	}
	else
	{
		wofstream out("config.dat");
		out << L"Program.exe|Inject.dll";	//delimeter |
		out.close();
		proc = L"Program.exe";
		dll = L"taek.dll";
	}
	file.close();
	wifstream f_dll(dll);
	if (!f_dll.is_open())
	{
		MessageBox(NULL, L"DLL 파일이 경로에 없습니다", L"ERROR", MB_ICONWARNING);
		return -1;
	}
	f_dll.close();
	wcout << L"Target Process : " << proc << endl;
	wcout << L"Target Dll : " << dll << endl;
	while (!Inject(proc, dll));
	MessageBox(NULL, L"인젝션 성공", L"야호", MB_ICONASTERISK);
	return 0;
}
