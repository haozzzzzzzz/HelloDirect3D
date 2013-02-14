#include <d3d9.h>


//�ͷ�Com�����
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}
#endif


wchar_t*g_pClassName=L"HelloDirect3D";//��������
wchar_t*g_pWindowName=L" ��Direct3Dʵ��";
LPDIRECT3DDEVICE9 g_pd3dDevice=NULL;//Direct3D�豸�ӿ�


HRESULT InitDirect3D(HWND hWnd);//��ʼ��Direct3D
void Direct3DRender();//��Ⱦͼ��
void Direct3DCleanup();//����Direct3D��Դ


//���ڹ��̺���
LRESULT CALLBACK WinMainProc(HWND,UINT,WPARAM,LPARAM);

//-----------------------------------------
//Name : WinMain();
//Desc : WindowsӦ�ó�����ں���
//----------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{
	WNDCLASS wndclass;
	//��ƴ�����
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hInstance=hInstance;
	wndclass.lpfnWndProc=WinMainProc;
	wndclass.lpszClassName=g_pClassName;
	wndclass.lpszMenuName=NULL;
	wndclass.style=CS_HREDRAW|CS_VREDRAW;

	//ע�ᴰ��
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL,L"ע�ᴰ��ʧ��",L"������ʾ",NULL);
		return 1;
	}

	//��������
	HWND hWnd=CreateWindow(g_pClassName,g_pWindowName,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,500,500,NULL,NULL,hInstance,NULL);
	if (!hWnd)
	{
		MessageBox(NULL,L"��������ʧ��",L"������ʾ",NULL);
		return 1;
	}

	//��ʼ�� Direct3D
	InitDirect3D(hWnd);


	//��ʾ����
	ShowWindow(hWnd,SW_SHOWNORMAL);
	
	//���´���
	UpdateWindow(hWnd);

	//������Ϣ
	MSG msg;
	ZeroMemory(&msg,sizeof(msg));
	while(msg.message!=WM_QUIT)
	{
		//����Ϣ������ȡ��Ϣ��ɾ����������Ϣ
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);//ת����Ϣ
			DispatchMessage(&msg);//�ַ���Ϣ
		}
		else
		{
			Direct3DRender();//����3D����
		}
	}

	UnregisterClass(g_pClassName,hInstance);
	return (int)msg.wParam;
}


//���ڹ��̺���
LRESULT CALLBACK WinMainProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:	//�ͻ�����h
		Direct3DRender();//��Ⱦͼ��
		ValidateRect(hWnd,NULL);//���¿ͻ�����ʾ
		break;
	case WM_DESTROY://����������Ϣ
		Direct3DCleanup();//����Direct3D
		PostQuitMessage(0);//�˳�
		break;
	case WM_KEYDOWN:
		if (wParam==VK_ESCAPE)//ESC��
		{
			DestroyWindow(hWnd);//���ٴ��ڣ�������һ��WM_DESTROY��Ϣ
		}
		break;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
		break;
	}
	return 0;
}


//Direct3D��ʼ��
HRESULT InitDirect3D(HWND hWnd)
{
	//����IDirect3D�ӿ�
	LPDIRECT3D9 pD3D=NULL;//IDirect3D9�ӿ�
	pD3D=Direct3DCreate9(D3D_SDK_VERSION);//����IDirect3D9�ӿڶ���
	//D3D_SDK_VERSION������ʾ��ǰʹ�õ�DirectX SDK�汾������ȷ��Ӧ�ó������а�����ͷ�ļ��ڱ���ʱ�ܹ���DirectX����ʱ��DLL��ƥ��

	if (pD3D==NULL)
	{
		return E_FAIL;
	}


	//��ȡӲ���豸��Ϣ
	//IDirect3D9�ӿ��ṩ��GetDeviceCaps������ȡָ���豸�����ܲ������÷�������ȡ�õ�Ӳ���豸��Ϣ���浽һ��D3DCAPS9�ṹ��
	/*
	HRESULT IDirect3D9::GetDeviceCaps( 
	UINT  Adapter, 
	D3DDEVTYPE  DeviceType, 
	D3DCAPS9  *pCaps 
	);
	*/
	D3DCAPS9 caps;
	int vp=0;
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
		vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;




	//����Direct3D�豸�ӿ�
	//�ڴ���IDirect3DDevice9�ӿڶ���֮ǰ����Ҫ׼��һ��D3DPRESENT_PARAMETERS�ṹ���͵Ĳ���������˵����δ���Direct3D�豸
	/*
	typedef struct _D3DPRESENT_PARAMETERS_ {
	UINT 			BackBufferWidth;//��̨���������
	UINT 			BackBufferHeight; //��̨�������߶�
	D3DFORMAT   		BackBufferFormat;//...�������صĸ�ʽ
	UINT 			BackBufferCount;//..����
	D3DMULTISAMPLE_TYPE  	MultiSampleType;//����ȡ������
	DWORD   			MultiSampleQuality;//����ȡ������
	D3DSWAPEFFECT  		SwapEffect;//��̨���������ݸ��Ƶ�ǰ̨�������ķ�ʽ
	HWND 			hDeviceWindow;//����ͼ�εĴ��ھ��
	BOOL 			Windowed;
	BOOL 			EnableAutoDepthStencil;//Direct3D�Ƿ�ΪӦ�ó����Զ������ڴ����
	D3DFORMAT   		AutoDepthStencilFormat;//��ʽ
	DWORD   			Flags;//�������� ͨ��Ϊ0
	UINT 			FullScreen_RefreshRateInHz;//ȫ��ģʽʱָ����Ļˢ����
	UINT 			PresentationInterval;//ָ��ƽ��ķ�תģʽ���ڴ���ģʽ�� ��ȡֵΪ0
	} D3DPRESENT_PARAMETERS;
	*/
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));


	d3dpp.BackBufferWidth=640;
	d3dpp.BackBufferHeight=480;
	d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount=1;
	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality=0;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=hWnd;
	d3dpp.Windowed=TRUE;
	d3dpp.EnableAutoDepthStencil=TRUE;
	d3dpp.AutoDepthStencilFormat=D3DFMT_D24S8;
	d3dpp.Flags=0;
	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;

	//�����D3DPRESENT_PARAMETERS�ṹ�󣬿��Ե���IDirect3D9�ӿڵ�CreateDeivce��������IDirect3DDevice9�ӿڶ���
	/*
	HRESULT IDirect3DDevice9::CreateDevice( 
	UINT     Adapter, //��ʾ��������IDirect3DDevice9�ӿڶ�����������Կ�����
	D3DDEVTYPE  DeviceType, 
	HWND   hFocusWindow, 
	DWORD  BehaviorFlags, //Direct3D�豸����3D����ķ�ʽ
	D3DPRESENT_PARAMETERS  *pPresentationParameters,
	IDirect3DDevice9  **ppReturnedDeviceInterface 
	);
	*/
	//IDirect3D9�ӿڵ�CreateDevice��������һ��HRESULT���͵ķ���ֵ������ͨ��SUCCESSED��FALIED���жϸú�����ִ�н�������CreateDevice����ִ�гɹ���SUCCESSED�꽫����TRUE����FAILED���򷵻�FALSE��
	pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,vp,&d3dpp,&g_pd3dDevice);

	pD3D->Release();//�ͷ�Direct3D�ӿ�

	return S_OK;

}


//---------------------------------------
//Name : Direct3DRender()
//Desc : ����3D����
//---------------------------------------
void Direct3DRender()
{
	/*
	Direct3D�Ļ��ƹ��̾��ǣ����ơ���ʾ�����ơ���ʾ��
	���ǣ�ÿ����ʼ����ͼ��֮ǰ������Ҫͨ��IDirect3DDevice9�ӿڵ�Clear��������̨�����е����ݽ�����գ������ñ���������ɫ��

	HRESULT IDirect3DDevice9::Clear( 
	DWORD   Count,
	const D3DRECT *pRects, //ָ���Ա���ָ���ľ������������������������а����ľ��ε�������Count����ָ��
	DWORD    Flags, //ָ������Ҫ����ı��棬�ò�������ȡֵ��D3DCLEAR_TARGET��D3DCLEAR_ZBUFFER��D3DCLEAR_STENCIL���ֱ��ʾ�Ժ�̨���桢��Ȼ����ģ�建��������
	D3DCOLOR Color, //����ָ��������������ݺ����õı���ɫ������ͨ��D3DCOLOR_XRGB�꽫RGBֵת�����ò���
	float        Z, //ָ��������������ݺ�������Ȼ����ֵ
	DWORD    Stencil 
	); 

	*/
	g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_XRGB(45,50,170),1.0f,0);//��ɫ

	//��ʼ����
	g_pd3dDevice->BeginScene();

	/*ͼ�λ��Ƶ�ʵ�ʹ���*/

	//��������
	g_pd3dDevice->EndScene();

	//��ת
	g_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}








//-----------------------------------------------
//Name : Direct3DCleanup()
//Desc : ����Direct3D , ���ͷ�COM�ӿ�
//----------------------------------------------
void Direct3DCleanup()
{
	SAFE_RELEASE(g_pd3dDevice);
}