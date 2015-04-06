////--------------------------------------------------------------------------------------
//// File: Tutorial06.cpp
////
//// This application demonstrates simple lighting in the vertex shader
////
//// http://msdn.microsoft.com/en-us/library/windows/apps/ff729723.aspx
////
//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved.
////--------------------------------------------------------------------------------------
//#include <windows.h>
//#include <d3d11_1.h>
//#include <d3dcompiler.h>
//#include <directxmath.h>
//#include <directxcolors.h>
//#include "resource.h"
//#include "CKinect.h"
//#include <OVR_CAPI.h>
//
//#define   OVR_D3D_VERSION 11
//#include "OVR_CAPI_D3D.h"
//
//using namespace DirectX;
//
////--------------------------------------------------------------------------------------
//// Structures
////--------------------------------------------------------------------------------------
//struct SimpleVertex
//{
//	XMFLOAT3 Pos;
//};
//
//
//struct ConstantBuffer
//{
//	XMMATRIX mWorld;
//	XMMATRIX mView;
//	XMMATRIX mProjection;
//	XMFLOAT4 vLightDir[2];
//	XMFLOAT4 vLightColor[2];
//	XMFLOAT4 vOutputColor;
//};
//
//
////--------------------------------------------------------------------------------------
//// Global Variables
////--------------------------------------------------------------------------------------
//HINSTANCE               g_hInst = nullptr;
//HWND                    g_hWnd = nullptr;
//D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
//D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
//ID3D11Device*           g_pd3dDevice = nullptr;
//ID3D11Device1*          g_pd3dDevice1 = nullptr;
//ID3D11DeviceContext*    g_pImmediateContext = nullptr;
//ID3D11DeviceContext1*   g_pImmediateContext1 = nullptr;
//IDXGISwapChain*         g_pSwapChain = nullptr;
//IDXGISwapChain1*        g_pSwapChain1 = nullptr;
//ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
//ID3D11Texture2D*        g_pDepthStencil = nullptr;
//ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
//ID3D11VertexShader*     g_pVertexShader = nullptr;
//ID3D11PixelShader*      g_pPixelShader = nullptr;
//ID3D11PixelShader*      g_pPixelShaderSolid = nullptr;
//ID3D11InputLayout*      g_pVertexLayout = nullptr;
//ID3D11Buffer*           g_pVertexBuffer = nullptr;
//ID3D11Buffer*           g_pIndexBuffer = nullptr;
//ID3D11Buffer*           g_pConstantBuffer = nullptr;
//XMMATRIX                g_World;
//XMMATRIX                g_View;
//XMMATRIX                g_Projection;
//
//ovrD3D11Texture EyeTexture[2];
//ovrEyeRenderDesc *EyeRenderDesc;
//
//
//// Create vertex buffe
//
//SimpleVertex vertices[(cDepthWidth * cDepthHeight)];
//
//// Create index buffer
//// Create vertex buffer
//DWORD indices[(cDepthWidth * cDepthHeight * 6)];
//
//
////--------------------------------------------------------------------------------------
//// Forward declarations
////--------------------------------------------------------------------------------------
//HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
//HRESULT InitDevice();
//void CleanupDevice();
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//void Render();
//
//
////oculus stuff
//ovrHmd hmd;
//
//
//
////--------------------------------------------------------------------------------------
//// Entry point to the program. Initializes everything and goes into a message processing 
//// loop. Idle time is used to render the scene.
////--------------------------------------------------------------------------------------
//int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
//{
//	//KINECT
//	InitializeDefaultSensor();
//
//	//oculus
//	ovr_Initialize();
//	hmd = ovrHmd_Create(0);
//	if (nullptr == hmd) {
//		hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
//	}
//
//	// Start the sensor which provides the Rift’s pose and motion.
//	ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
//		ovrTrackingCap_MagYawCorrection |
//		ovrTrackingCap_Position, 0);
//	
//
//
//
//
//	UNREFERENCED_PARAMETER(hPrevInstance);
//	UNREFERENCED_PARAMETER(lpCmdLine);
//
//	if (FAILED(InitWindow(hInstance, nCmdShow)))
//		return 0;
//
//	if (FAILED(InitDevice()))
//	{
//		CleanupDevice();
//		return 0;
//	}
//
//	// Main message loop
//	MSG msg = { 0 };
//	while (WM_QUIT != msg.message)
//	{
//		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//		else
//		{
//			Render();
//		}
//	}
//
//	CleanupDevice();
//
//	return (int)msg.wParam;
//}
//
//
////--------------------------------------------------------------------------------------
//// Register class and create window
////--------------------------------------------------------------------------------------
//HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
//{
//	// Register class
//	WNDCLASSEX wcex;
//	wcex.cbSize = sizeof(WNDCLASSEX);
//	wcex.style = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc = WndProc;
//	wcex.cbClsExtra = 0;
//	wcex.cbWndExtra = 0;
//	wcex.hInstance = hInstance;
//	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
//	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wcex.lpszMenuName = nullptr;
//	wcex.lpszClassName = L"TutorialWindowClass";
//	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
//	if (!RegisterClassEx(&wcex))
//		return E_FAIL;
//
//	// Create window
//	g_hInst = hInstance;
//
//	g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 6",
//		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
//		hmd->WindowsPos.x, hmd->WindowsPos.y, hmd->Resolution.w, hmd->Resolution.h, nullptr, nullptr, hInstance,
//		nullptr);
//	if (!g_hWnd)
//		return E_FAIL;
//
//	ShowWindow(g_hWnd, nCmdShow);
//
//	return S_OK;
//}
//
//
////--------------------------------------------------------------------------------------
//// Helper for compiling shaders with D3DCompile
////
//// With VS 11, we could load up prebuilt .cso files instead...
////--------------------------------------------------------------------------------------
//HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
//{
//	HRESULT hr = S_OK;
//
//	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
//#ifdef _DEBUG
//	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
//	// Setting this flag improves the shader debugging experience, but still allows 
//	// the shaders to be optimized and to run exactly the way they will run in 
//	// the release configuration of this program.
//	dwShaderFlags |= D3DCOMPILE_DEBUG;
//
//	// Disable optimizations to further improve shader debugging
//	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
//#endif
//
//	ID3DBlob* pErrorBlob = nullptr;
//	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
//		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
//	if (FAILED(hr))
//	{
//		if (pErrorBlob)
//		{
//			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
//			pErrorBlob->Release();
//		}
//		return hr;
//	}
//	if (pErrorBlob) pErrorBlob->Release();
//
//	return S_OK;
//}
//
//
////--------------------------------------------------------------------------------------
//// Create Direct3D device and swap chain
////--------------------------------------------------------------------------------------
//HRESULT InitDevice()
//{
//	HRESULT hr = S_OK;
//
//	RECT rc;
//	GetClientRect(g_hWnd, &rc);
//	UINT width = rc.right - rc.left;
//	UINT height = rc.bottom - rc.top;
//
//	UINT createDeviceFlags = 0;
//#ifdef _DEBUG
//	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//
//	D3D_DRIVER_TYPE driverTypes[] =
//	{
//		D3D_DRIVER_TYPE_HARDWARE,
//		D3D_DRIVER_TYPE_WARP,
//		D3D_DRIVER_TYPE_REFERENCE,
//	};
//	UINT numDriverTypes = ARRAYSIZE(driverTypes);
//
//	D3D_FEATURE_LEVEL featureLevels[] =
//	{
//		D3D_FEATURE_LEVEL_11_1,
//		D3D_FEATURE_LEVEL_11_0,
//		D3D_FEATURE_LEVEL_10_1,
//		D3D_FEATURE_LEVEL_10_0,
//	};
//	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
//
//	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
//	{
//		g_driverType = driverTypes[driverTypeIndex];
//		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
//			D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
//
//		if (hr == E_INVALIDARG)
//		{
//			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
//			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
//				D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
//		}
//
//		if (SUCCEEDED(hr))
//			break;
//	}
//	if (FAILED(hr))
//		return hr;
//
//	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
//	IDXGIFactory1* dxgiFactory = nullptr;
//	{
//		IDXGIDevice* dxgiDevice = nullptr;
//		hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
//		if (SUCCEEDED(hr))
//		{
//			IDXGIAdapter* adapter = nullptr;
//			hr = dxgiDevice->GetAdapter(&adapter);
//			if (SUCCEEDED(hr))
//			{
//				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
//				adapter->Release();
//			}
//			dxgiDevice->Release();
//		}
//	}
//	if (FAILED(hr))
//		return hr;
//
//	// Create swap chain
//	IDXGIFactory2* dxgiFactory2 = nullptr;
//	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
//	if (dxgiFactory2)
//	{
//		// DirectX 11.1 or later
//		hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
//		if (SUCCEEDED(hr))
//		{
//			(void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1));
//		}
//
//		DXGI_SWAP_CHAIN_DESC1 sd;
//		ZeroMemory(&sd, sizeof(sd));
//		sd.Width = width;
//		sd.Height = height;
//		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		sd.SampleDesc.Count = 1;
//		sd.SampleDesc.Quality = 0;
//		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		sd.BufferCount = 1;
//
//		hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
//		if (SUCCEEDED(hr))
//		{
//			hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));
//		}
//
//		dxgiFactory2->Release();
//	}
//	else
//	{
//		// DirectX 11.0 systems
//		DXGI_SWAP_CHAIN_DESC sd;
//		ZeroMemory(&sd, sizeof(sd));
//		sd.BufferCount = 1;
//		sd.BufferDesc.Width = width;
//		sd.BufferDesc.Height = height;
//		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		sd.BufferDesc.RefreshRate.Numerator = 60;
//		sd.BufferDesc.RefreshRate.Denominator = 1;
//		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		sd.OutputWindow = g_hWnd;
//		sd.SampleDesc.Count = 1;
//		sd.SampleDesc.Quality = 0;
//		sd.Windowed = TRUE;
//
//		hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
//	}
//
//	// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
//	dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);
//
//	dxgiFactory->Release();
//
//	if (FAILED(hr))
//		return hr;
//
//
//	//oculus
//	// Configure Stereo settings.
//	ovrSizei recommenedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left,
//		hmd->DefaultEyeFov[0], 1.0f);
//	ovrSizei recommenedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right,
//		hmd->DefaultEyeFov[1], 1.0f);
//	ovrSizei renderTargetSize;
//	renderTargetSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
//	renderTargetSize.h = max(recommenedTex0Size.h, recommenedTex1Size.h);
//	const int eyeRenderMultisample = 1;
//	pRendertargetTexture = pRender->CreateTexture(
//		Texture_RGBA | Texture_RenderTarget | eyeRenderMultisample,
//		renderTargetSize.w, renderTargetSize.h, NULL);
//	// The actual RT size may be different due to HW limits.
//	renderTargetSize.w = pRendertargetTexture->GetWidth();
//	renderTargetSize.h = pRendertargetTexture->GetHeight();
//
//	// Create a render target view
//	ID3D11Texture2D* pBackBuffer = nullptr;
//	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
//	if (FAILED(hr))
//		return hr;
//
//	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
//	pBackBuffer->Release();
//	if (FAILED(hr))
//		return hr;
//
//	// Configure D3D11.
//	ovrD3D11Config d3d11cfg;
//	d3d11cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
//	ovrSizei res; res.h = hmd->Resolution.h; res.w = hmd->Resolution.w;
//	d3d11cfg.D3D11.Header.BackBufferSize = ovrSizei(res);
//	d3d11cfg.D3D11.Header.Multisample = 1;
//	d3d11cfg.D3D11.pDevice = g_pd3dDevice;
//	d3d11cfg.D3D11.pDeviceContext = g_pImmediateContext;
//	d3d11cfg.D3D11.pBackBufferRT = g_pRenderTargetView;
//	d3d11cfg.D3D11.pSwapChain = g_pSwapChain;
//
//	if (!ovrHmd_ConfigureRendering(hmd, &d3d11cfg.Config,
//		ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette |
//		ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive,
//		hmd->DefaultEyeFov, EyeRenderDesc))
//
//	// Create depth stencil texture
//	D3D11_TEXTURE2D_DESC descDepth;
//	ZeroMemory(&descDepth, sizeof(descDepth));
//	descDepth.Width = width;
//	descDepth.Height = height;
//	descDepth.MipLevels = 1;
//	descDepth.ArraySize = 1;
//	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	descDepth.SampleDesc.Count = 1;
//	descDepth.SampleDesc.Quality = 0;
//	descDepth.Usage = D3D11_USAGE_DEFAULT;
//	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	descDepth.CPUAccessFlags = 0;
//	descDepth.MiscFlags = 0;
//	hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
//	if (FAILED(hr))
//		return hr;
//
//	// Create the depth stencil view
//	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
//	ZeroMemory(&descDSV, sizeof(descDSV));
//	descDSV.Format = descDepth.Format;
//	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	descDSV.Texture2D.MipSlice = 0;
//	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
//	if (FAILED(hr))
//		return hr;
//
//	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
//
//	// Setup the viewport
//	D3D11_VIEWPORT vp;
//	vp.Width = (FLOAT)width;
//	vp.Height = (FLOAT)height;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	g_pImmediateContext->RSSetViewports(1, &vp);
//
//	// Compile the vertex shader
//	ID3DBlob* pVSBlob = nullptr;
//	hr = CompileShaderFromFile(L"Tutorial06.fx", "VS", "vs_4_0", &pVSBlob);
//	if (FAILED(hr))
//	{
//		MessageBox(nullptr,
//			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
//		return hr;
//	}
//
//	// Create the vertex shader
//	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
//	if (FAILED(hr))
//	{
//		pVSBlob->Release();
//		return hr;
//	}
//
//	// Define the input layout
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	UINT numElements = ARRAYSIZE(layout);
//
//	// Create the input layout
//	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
//		pVSBlob->GetBufferSize(), &g_pVertexLayout);
//	pVSBlob->Release();
//	if (FAILED(hr))
//		return hr;
//
//	// Set the input layout
//	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
//
//	// Compile the pixel shader
//	ID3DBlob* pPSBlob = nullptr;
//	hr = CompileShaderFromFile(L"Tutorial06.fx", "PS", "ps_4_0", &pPSBlob);
//	if (FAILED(hr))
//	{
//		MessageBox(nullptr,
//			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
//		return hr;
//	}
//
//	// Create the pixel shader
//	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
//	pPSBlob->Release();
//	if (FAILED(hr))
//		return hr;
//
//	// Compile the pixel shader
//	pPSBlob = nullptr;
//	hr = CompileShaderFromFile(L"Tutorial06.fx", "PSSolid", "ps_4_0", &pPSBlob);
//	if (FAILED(hr))
//	{
//		MessageBox(nullptr,
//			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
//		return hr;
//	}
//
//	// Create the pixel shader
//	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShaderSolid);
//	pPSBlob->Release();
//	if (FAILED(hr))
//		return hr;
//
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = (sizeof(CameraSpacePoint) * cDepthWidth * cDepthHeight);
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = vertices;
//	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
//	if (FAILED(hr))
//		return hr;
//
//	// Set vertex buffer
//	UINT stride = sizeof(SimpleVertex);
//	UINT offset = 0;
//	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
//
//
//	//initialize the index buffer
//	for (int i = 0; i < (cDepthWidth * cDepthHeight) - cDepthWidth; i++){
//
//		int n = i * 6;
//		indices[n] = i;
//		indices[n + 1] = i + 1;
//		indices[n + 2] = i + cDepthWidth;
//
//		indices[n + 3] = i + 1;
//		indices[n + 4] = i + cDepthWidth;
//		indices[n + 5] = i + cDepthWidth + 1;
//
//	}
//
//	//set index buffer
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = cDepthWidth * cDepthHeight * 6 * sizeof(DWORD);  
//	bd.StructureByteStride = 4;
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	InitData.pSysMem = indices;
//	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
//	if (FAILED(hr))
//		return hr;
//
//	
//
//	// Set index buffer
//	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// Set primitive topology
//	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	//g_pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
//
//	// Create the constant buffer
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(ConstantBuffer);
//	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	bd.CPUAccessFlags = 0;
//	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
//	if (FAILED(hr))
//		return hr;
//
//	// Initialize the world matrices
//	g_World = XMMatrixIdentity();
//
//	// Initialize the view matrix
//	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
//	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
//	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//	g_View = XMMatrixLookAtLH(Eye, At, Up);
//
//	// Initialize the projection matrix
//	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
//
//	return S_OK;
//}
//
//
////--------------------------------------------------------------------------------------
//// Clean up the objects we've created
////--------------------------------------------------------------------------------------
//void CleanupDevice()
//{
//	if (g_pImmediateContext) g_pImmediateContext->ClearState();
//
//	if (g_pConstantBuffer) g_pConstantBuffer->Release();
//	if (g_pVertexBuffer) g_pVertexBuffer->Release();
//	if (g_pIndexBuffer) g_pIndexBuffer->Release();
//	if (g_pVertexLayout) g_pVertexLayout->Release();
//	if (g_pVertexShader) g_pVertexShader->Release();
//	if (g_pPixelShaderSolid) g_pPixelShaderSolid->Release();
//	if (g_pPixelShader) g_pPixelShader->Release();
//	if (g_pDepthStencil) g_pDepthStencil->Release();
//	if (g_pDepthStencilView) g_pDepthStencilView->Release();
//	if (g_pRenderTargetView) g_pRenderTargetView->Release();
//	if (g_pSwapChain1) g_pSwapChain1->Release();
//	if (g_pSwapChain) g_pSwapChain->Release();
//	if (g_pImmediateContext1) g_pImmediateContext1->Release();
//	if (g_pImmediateContext) g_pImmediateContext->Release();
//	if (g_pd3dDevice1) g_pd3dDevice1->Release();
//	if (g_pd3dDevice) g_pd3dDevice->Release();
//}
//
//
////--------------------------------------------------------------------------------------
//// Called every time the application receives a message
////--------------------------------------------------------------------------------------
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	PAINTSTRUCT ps;
//	HDC hdc;
//
//	switch (message)
//	{
//	case WM_KEYUP:
//		ovrHmd_RecenterPose(hmd);
//	case WM_PAINT:
//		hdc = BeginPaint(hWnd, &ps);
//		EndPaint(hWnd, &ps);
//		break;
//
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//
//		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
//		// so we created the window without the resize border.
//
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//
//	return 0;
//}
//
//XMMATRIX poseToMatrix(const ovrPosef & op) {
//	
//	XMFLOAT4 ori = XMFLOAT4(op.Orientation.x, op.Orientation.y, op.Orientation.z, op.Orientation.w);
//	XMFLOAT3 pos = XMFLOAT3(op.Position.x, op.Position.y, op.Position.z);
//	XMMATRIX orientation = XMMatrixRotationQuaternion(XMLoadFloat4(&ori));
//	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&pos));
//	return XMMatrixTranspose(translation * orientation);
//}
//
//XMMATRIX getRiftCameraMatrix(ovrHmd hmd) {
//
//	ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());
//	if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
//	{
//		ovrPoseStatef pose = ts. HeadPose;
//		return poseToMatrix(pose.ThePose);
//	}
//}
//
//
////--------------------------------------------------------------------------------------
//// Render a frame
////--------------------------------------------------------------------------------------
//void Render()
//{
//	//kinect
//	update();
//
//	//oculus
//	ovrHmd_BeginFrame(hmd, 0);
//
//
//
//	g_pImmediateContext->UpdateSubresource(g_pVertexBuffer, 0, 0, kinectVerticies, cDepthHeight * cDepthWidth * sizeof(CameraSpacePoint), 0);
//
//	// Update our time
//	static float t = 0.0f;
//	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
//	{
//		t += (float)XM_PI * 0.0125f;
//	}
//	else
//	{
//		static ULONGLONG timeStart = 0;
//		ULONGLONG timeCur = GetTickCount64();
//		if (timeStart == 0)
//			timeStart = timeCur;
//		t = (timeCur - timeStart) / 1000.0f;
//	}
//
//	// Rotate cube around the origin
//	g_World = XMMatrixIdentity();
//
//	// Setup our lighting parameters
//	XMFLOAT4 vLightDirs[2] =
//	{
//		XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
//		XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),
//	};
//	XMFLOAT4 vLightColors[2] =
//	{
//		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
//		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f)
//	};
//
//	// Rotate the second light around the origin
//	XMMATRIX mRotate = XMMatrixRotationY(-2.0f * t);
//	XMVECTOR vLightDir = XMLoadFloat4(&vLightDirs[1]);
//	vLightDir = XMVector3Transform(vLightDir, mRotate);
//	XMStoreFloat4(&vLightDirs[1], vLightDir);
//
//	//
//	// Clear the back buffer
//	//
//
//	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);
//
//	//
//	// Clear the depth buffer to 1.0 (max depth)
//	//
//	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
//
//	//
//	// Update matrix variables and lighting variables
//	//
//	ConstantBuffer cb1;
//	cb1.mWorld = XMMatrixTranspose(g_World);
//	cb1.mView = getRiftCameraMatrix(hmd);// XMMatrixTranspose(g_View);
//	cb1.mProjection = XMMatrixTranspose(g_Projection);
//	cb1.vLightDir[0] = vLightDirs[0];
//	cb1.vLightDir[1] = vLightDirs[1];
//	cb1.vLightColor[0] = vLightColors[0];
//	cb1.vLightColor[1] = vLightColors[1];
//	cb1.vOutputColor = XMFLOAT4(0, 0, 0, 1);
//	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb1, 0, 0);
//
//	//
//	// Render the cube
//	//
//	g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
//	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
//	g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
//	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
//	
//	
//	g_pImmediateContext->DrawIndexed((cDepthHeight * cDepthWidth * 6), 0, 0);
//
//
//	// Pass D3D11 texture data, including ID3D11Texture2D and ID3D11ShaderResourceView pointers.
//	EyeTexture[0].D3D11.Header.API = ovrRenderAPI_D3D11;
//	EyeTexture[0].D3D11.Header.TextureSize = RenderTargetSize;
//	EyeTexture[0].D3D11.Header.RenderViewport = EyeRenderViewport[0];
//	EyeTexture[0].D3D11.pTexture = RendertargetTexture;
//	EyeTexture[0].D3D11.pSRView = RendertargetSRV;
//	// Right eye uses the same texture information but a different rendering viewport.
//	EyeTexture[1] = EyeTexture[0];
//	EyeTexture[1].D3D11.Header.RenderViewport = EyeRenderViewport[1];
//
//	ovrHmd_EndFrame(hmd, 0);
//}
//
//
