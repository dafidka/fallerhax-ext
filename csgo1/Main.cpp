#include <Windows.h>
#include <TlHelp32.h>
#include "offsets.h"
//#include <iostream>
//#include <string>
#include <future>





using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

DWORD GetPID(const char* ProcessName) {
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);


	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!strcmp(processInfo.szExeFile, ProcessName))
	{
		CloseHandle(processesSnapshot);
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!strcmp(processInfo.szExeFile, ProcessName))
		{
			CloseHandle(processesSnapshot);
		}
	}
	CloseHandle(processesSnapshot);
	return processInfo.th32ProcessID;
}

DWORD* GetModule(const char* moduleName, unsigned long ProcessID)
{
	MODULEENTRY32 modEntry = { 0 };

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);

	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 curr = { 0 };

		curr.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &curr))
		{
			do
			{
				if (!strcmp(curr.szModule, moduleName))
				{
					modEntry = curr;
					break;
				}
			} while (Module32Next(hSnapshot, &curr));
		}
		CloseHandle(hSnapshot);
	}
	return reinterpret_cast<DWORD*>(modEntry.modBaseAddr);
}


/*void fov(bool enabledFovChanger, bool isScoped, HANDLE pHandle, DWORD localP, DWORD myFov, DWORD defaultFov)
{
	if (enabledFovChanger) {
		if (!isScoped)
		{
			WriteProcessMemory(pHandle, (LPVOID)(localP + m_iFOV), &myFov, sizeof(myFov), 0);
			//WriteProcessMemory(pHandle, (LPVOID)(localP + m_iFOVStart), &myFov, sizeof(myFov), 0);

		}
	}
	else
	{
		if (isScoped)
		{
			//WriteProcessMemory(pHandle, (LPVOID)(localP + m_iFOV), &defaultFov, sizeof(defaultFov), 0);
			//WriteProcessMemory(pHandle, (LPVOID)(localP + m_iFOVStart), &myFov, sizeof(myFov), 0);

		}
	}
};*/

void trigger(HANDLE pHandle, DWORD localP, int lookingAt, DWORD baseAddr, DWORD addrOfLookingAt, DWORD playerInCrossHealth, DWORD playerInCrossTeam, int myTeam, int forceattackdown, int forceattackup)
{
	if (GetAsyncKeyState(VK_XBUTTON1) || GetAsyncKeyState(VK_MENU))
	{

		ReadProcessMemory(pHandle, (LPCVOID)(localP + m_iCrosshairId), &lookingAt, sizeof(lookingAt), 0);

		//std::cout << "myTeam: " << myTeam << std::endl;
		//std::cout << "lookingAt: " << lookingAt <<std::endl;

		if (0 > lookingAt < 65)
		{
			ReadProcessMemory(pHandle, (LPCVOID)(baseAddr + dwEntityList + (lookingAt - 1) * 16), &addrOfLookingAt, sizeof(addrOfLookingAt), 0);  //getting the playeraddress of lookingAt
			ReadProcessMemory(pHandle, (LPCVOID)(addrOfLookingAt + m_iHealth), &playerInCrossHealth, sizeof(playerInCrossHealth), 0); //calcuating the health of lookingat with playeraddress
			ReadProcessMemory(pHandle, (LPCVOID)(addrOfLookingAt + m_iTeamNum), &playerInCrossTeam, sizeof(playerInCrossTeam), 0);
			//std::cout << ptroflookingAt << std::endl;

			if ((playerInCrossHealth > 0) && playerInCrossTeam != myTeam)
			{

				WriteProcessMemory(pHandle, (LPVOID)(baseAddr + dwForceAttack), &forceattackdown, sizeof(forceattackdown), 0);
				Sleep(1);
				WriteProcessMemory(pHandle, (LPVOID)(baseAddr + dwForceAttack), &forceattackup, sizeof(forceattackup), 0);

			}
		}
	}
}

void glowhac(bool enabledGlow, HANDLE pHandle, DWORD baseAddr, int entity, int glow, int entityTeam, bool hasDefuser, int myTeam, float r, float g, float b, float a, DWORD glowObject, bool igaz, bool hamis)
{

	if (enabledGlow == true)
	{
		for (short int i = 0; i < 33; i++)
		{
			ReadProcessMemory(pHandle, (LPCVOID)(baseAddr + dwEntityList + i * 16), &entity, sizeof(entity), 0);
			if (entity != NULL)
			{
				ReadProcessMemory(pHandle, (LPCVOID)(entity + m_iGlowIndex), &glow, sizeof(glow), 0);
				ReadProcessMemory(pHandle, (LPCVOID)(entity + m_iTeamNum), &entityTeam, sizeof(entityTeam), 0);
				ReadProcessMemory(pHandle, (LPCVOID)(entity + m_bHasDefuser), &hasDefuser, sizeof(hasDefuser), 0);

				if (entityTeam == myTeam)
				{
					r = 0;
					g = 2;
					b = 0.5;
					a = 0.5;

					WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x4), &r, sizeof(r), 0);
					WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x8), &g, sizeof(g), 0);
					WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0xC), &b, sizeof(b), 0);
					WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x10), &a, sizeof(a), 0);
				}
				else
				{
					if (hasDefuser == 0)
					{
						r = 0.5;
						g = 0;
						b = 2;
						a = 0.5;

						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x4), &r, sizeof(r), 0);
						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x8), &g, sizeof(g), 0);
						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0xC), &b, sizeof(b), 0);
						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x10), &a, sizeof(a), 0);
					}

					if (hasDefuser == 1)
					{
						r = 1;
						g = 0;
						b = 2;
						a = 0.5;

						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x4), &r, sizeof(r), 0);
						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x8), &g, sizeof(g), 0);
						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0xC), &b, sizeof(b), 0);
						WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x10), &a, sizeof(a), 0);

					}

				}
				WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x24), &igaz, sizeof(igaz), 0);
				WriteProcessMemory(pHandle, (LPVOID)(glowObject + glow * 0x38 + 0x25), &hamis, sizeof(hamis), 0);

				//Sleep(0.5);

			}
		}

	}
}

void bhopp(HANDLE pHandle, DWORD localP, int flags, bool enabledBhop, DWORD jump, DWORD baseAddr)
{
bhop:
	ReadProcessMemory(pHandle, (LPCVOID)(localP + m_fFlags), &flags, sizeof(flags), 0);
	//std::cout << "fFlags: "<< flags << std::endl;
	if (enabledBhop) {
		while (GetAsyncKeyState(VK_SPACE) && flags == 257)
		{
			jump = 5;
			WriteProcessMemory(pHandle, (LPVOID)(baseAddr + dwForceJump), &jump, sizeof(jump), 0);
			Sleep(1);
			jump = 4;
			WriteProcessMemory(pHandle, (LPVOID)(baseAddr + dwForceJump), &jump, sizeof(jump), 0);

			goto bhop;
		}
	}
}


int main()
{

	void ConsoleVisibility();
	{
		//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	};



	int processID = GetPID("csgo.exe");
	DWORD* module = GetModule("client.dll", processID);

	//HWND hWnd = FindWindowA(0, ("Counter-Strike:  Global Offensive"));
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
	//HWND hWnd = FindWindowA(0, ("Counter-Strike: Global Offensive"));

	DWORD baseAddr;
	DWORD localP;
	int flags;
	DWORD jump;
	DWORD addrOfLookingAt;
	baseAddr = reinterpret_cast<DWORD>(module);
	//int entityList;
	int myTeam;
	int lookingAt;
	DWORD playerInCrossHealth;
	DWORD playerInCrossTeam;
	int forceattackdown = 5;
	int forceattackup = 4;
	int entity;
	int entityTeam;
	DWORD glowObject;
	float r;
	float g;
	float b;
	float a;
	bool igaz = true;
	bool hamis = false;
	int glow;
	int myFov = 120;
	DWORD defaultFov;
	bool enabledTrigger = false;
	bool enabledGlow = false;
	bool enabledBhop = false;
	bool enabledTP = false;
	bool isScoped = false;
	int kett = 1;
	int ha = 4;
	int nulla = 0;
	int defusing = false;
	DWORD life;
	DWORD flash = 0;
	bool hasDefuser;
	int fovStart = 0;
	bool enabledFovChanger = false;
	int penalty;
	DWORD activeWeapon;
	DWORD lookAtWpn;

	ReadProcessMemory(pHandle, (LPCVOID)(baseAddr + dwLocalPlayer), &localP, sizeof(localP), 0);  //localP
	//ReadProcessMemory(pHandle, (LPVOID)(localP + m_iFOV), &defaultFov, sizeof(defaultFov), 0);

	while (true)
	{
		ReadProcessMemory(pHandle, (LPCVOID)(baseAddr + dwLocalPlayer), &localP, sizeof(localP), 0);  //localP
		ReadProcessMemory(pHandle, (LPCVOID)(localP + m_iTeamNum), &myTeam, sizeof(myTeam), 0);
		ReadProcessMemory(pHandle, (LPCVOID)(localP + m_lifeState), &life, sizeof(life), 0);
		ReadProcessMemory(pHandle, (LPCVOID)(baseAddr + dwGlowObjectManager), &glowObject, sizeof(glowObject), 0);
		//WriteProcessMemory(pHandle, (LPVOID)(localP + m_iFOV), &myFov, sizeof(myFov), 0);
		ReadProcessMemory(pHandle, (LPCVOID)(localP + m_bIsScoped), &isScoped, sizeof(isScoped), 0);
		WriteProcessMemory(pHandle, (LPVOID)(localP + m_flFlashDuration), &flash, sizeof(flash), 0);
		//ReadProcessMemory(pHandle, (LPCVOID)(localP + m_iFOVStart), &fovStart, sizeof(fovStart), 0);
		ReadProcessMemory(pHandle, (LPCVOID)(dwWeaponTable + m_hActiveWeapon), &activeWeapon, sizeof(activeWeapon), 0);
		ReadProcessMemory(pHandle, (LPCVOID)(activeWeapon + m_fAccuracyPenalty), &penalty, sizeof(penalty), 0);
		ReadProcessMemory(pHandle, (LPCVOID)(localP + m_iCrosshairId), &lookingAt, sizeof(lookingAt), 0);
		ReadProcessMemory(pHandle, (LPCVOID)(baseAddr + dwEntityList + (lookingAt -1)), &lookAtWpn, sizeof(lookAtWpn), 0);  //getting the playeraddress of lookingAt
		//lookAtWpn = lookAtWpn / 60;



		//std::future<void> fut = std::async(fov, enabledFovChanger, isScoped, pHandle, localP, myFov, defaultFov);

		std::future<void> fut2 = std::async(trigger, pHandle, localP, lookingAt, baseAddr, addrOfLookingAt, playerInCrossHealth, playerInCrossTeam, myTeam, forceattackdown, forceattackup);

		std::future<void> fut3 = std::async(glowhac, enabledGlow, pHandle, baseAddr, entity, glow, entityTeam, hasDefuser, myTeam, r, g, b, a, glowObject, igaz, hamis);

		std::future<void> fut4 = std::async(bhopp, pHandle, localP, flags, enabledBhop, jump, baseAddr);			//p90 activeWep = 9896230  //lookingAt = 412 //3288334446
																													//nova activeWep = 43450759 //lookingAt = 391

		//if (lookingAt > 0)
			//std::cout << "Looking at: " << lookAtWpn << std::endl;
		//std::cout << "In hand: " << activeWeapon << std::endl;
		//system("cls");

		switch (enabledGlow)
		{
		case false:
			if (GetAsyncKeyState(VK_INSERT))
			{
				enabledGlow = true;
				Sleep(100);
			}
		case true:
			if (GetAsyncKeyState(VK_INSERT))
			{
				enabledGlow = false;
				Sleep(100);
			}
			break;
		}

		switch (enabledTP)
		{
		case false:
			if (GetAsyncKeyState(VK_PRIOR))
			{
				enabledTP = true;
				Sleep(100);
			}
		case true:
			if (GetAsyncKeyState(VK_PRIOR))
			{
				enabledTP = false;
				Sleep(100);
			}
			break;
		}

		switch (enabledBhop)
		{
		case false:
			if (GetAsyncKeyState(VK_NEXT))
			{
				enabledBhop = true;
				Sleep(100);
			}
		case true:
			if (GetAsyncKeyState(VK_NEXT))
			{
				enabledBhop = false;
				Sleep(100);
			}
			break;
		}

		switch (enabledFovChanger)
		{
		case false:
			if (GetAsyncKeyState(VK_HOME))
			{
				enabledFovChanger = true;
				Sleep(100);
			}
		case true:
			if (GetAsyncKeyState(VK_HOME))
			{
				enabledFovChanger = false;
				Sleep(100);
			}
			break;
		}

		//std::cout << enabledGlow << std::endl;


		ReadProcessMemory(pHandle, (LPCVOID)(localP + m_bIsDefusing), &defusing, sizeof(defusing), 0);

		if (defusing == 1)
		{
			//std::cout << defusing << std::endl;
			WriteProcessMemory(pHandle, (LPVOID)(localP + m_iObserverMode), &kett, sizeof(kett), 0);
			//goto glow;
		}


		//if (defusing = false)
			//std::cout << defusing << std::endl;



		if (enabledTP == true && life == 0 && defusing == 0)
		{
			WriteProcessMemory(pHandle, (LPVOID)(localP + m_iObserverMode), &kett, sizeof(kett), 0);
		}
		if (enabledTP == false && life == 0 && defusing == 0)
		{
			WriteProcessMemory(pHandle, (LPVOID)(localP + m_iObserverMode), &nulla, sizeof(nulla), 0);
		}
		if (life > 0)
		{
			WriteProcessMemory(pHandle, (LPVOID)(localP + m_iObserverMode), &ha, sizeof(ha), 0);
		}


		//fut;
		fut2;
		fut3;
		fut4;


		if (GetAsyncKeyState(VK_END))
		{
			enabledBhop = false;
			enabledFovChanger = false;
			enabledGlow = false;
			enabledTP = false;
			enabledTrigger = false;
			Sleep(10);
			exit(0);
		}
	}

	//std::cout << fov << std::endl;
	//system("cls");       //only if debugging, makes bhop laggy
}






