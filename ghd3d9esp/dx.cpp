#include "includes.h"

//DEFINE_MEMBER_N(DWORD**, dwppDirect3DDevice9, hazedumper::signatures::dwppDirect3DDevice9);



int windowHeight, windowWidth;
//DWORD** dwppDirect3DDevice9 = (DWORD**)hazedumper::signatures::dwppDirect3DDevice9 + 0x1;

BOOL CALLBACK enumWind(HWND handle, LPARAM lp)
{
	DWORD procId;
	GetWindowThreadProcessId(handle, &procId);
	if (GetCurrentProcessId() != procId)
		return TRUE;

	window = handle;
	return FALSE;
}

HWND GetProcessWindow() 
{
	window = NULL;

	EnumWindows(enumWind, NULL);
	RECT size;
	GetWindowRect(window, &size); 
	windowWidth = size.right - size.left;
	windowHeight = size.bottom - size.top;

	//windowed
	//windowHeight -= 29;
	//windowWidth -= 5;

	return window;
}

bool GetD3D9Device(void** pTable, size_t size)
{

	DWORD d3device9 = hazedumper::signatures::dwppDirect3DDevice9 + 0x1;
	//void* testing[119] =  
	std::cout << "GetD3D9Device" << std::endl;
	if (!pTable)
		return false;
	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return false;
	IDirect3DDevice9* pDummyDevice = nullptr;

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();
//	uintptr_t client = (uintptr_t)GetModuleHandle("client.dll");
	// 
	std::cout << "About to create dummy dev" << std::endl;

	HRESULT dummyDevCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

	if (dummyDevCreated != S_OK)
	{
		d3dpp.Windowed = !d3dpp.Windowed;
		std::cout << "2nd dummy dev call" << std::endl;

		HRESULT dummyDevCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDevCreated != S_OK)
		{
			std::cout << "if you see this it broke" << std::endl;

			pD3D->Release();
			return false;
		}
	}
	//memcpy(pTable, *(void***)(pDummyDevice), size);
	
	memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), size);
	//memcpy(pTable, *(void***)(client + dwppDirect3DDevice9), size);
	//or 
	//memcpy(pTable, *(DWORD**)(client + dwppDirect3DDevice9), size);
	pDummyDevice->Release();
	pD3D->Release();
	return true;

}