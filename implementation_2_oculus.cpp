//
//		 /************************************************************************************
//		 This is a cut down, cleaned up version of the Oculus 0.4.4 SDK tiny room demo.
//		 Simplifications include SDK distortion and Direct to Rift support only.
//		 Modifications by Matt Newport - http://mattnewport.com/
//		 Original copyright notice below:
//		 ---
//		 Content     :   First-person view test application for Oculus Rift
//		 Created     :   October 4, 2012
//		 Authors     :   Tom Heath, Michael Antonov, Andrew Reisse, Volga Aksoy
//		 Copyright   :   Copyright 2012 Oculus, Inc. All Rights reserved.
//		 Licensed under the Apache License, Version 2.0 (the "License");
//		 you may not use this file except in compliance with the License.
//		 You may obtain a copy of the License at
//		 http://www.apache.org/licenses/LICENSE-2.0
//		 Unless required by applicable law or agreed to in writing, software
//		 distributed under the License is distributed on an "AS IS" BASIS,
//		 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//		 See the License for the specific language governing permissions and
//		 limitations under the License.
//		 *************************************************************************************/
//
//		 // This app renders a simple room, with right handed coord system :  Y->Up, Z->Back, X->Right
//		 // 'W','A','S','D' and arrow keys to navigate.
//
//#include <OVR_CAPI.h>  // Include the OculusVR SDK
//#include <Kernel/OVR_Math.h>
//
//#include <comdef.h>
//#include <comip.h>
//
//#include <d3d11.h>
//#include <d3dcompiler.h>
//
//#define OVR_D3D_VERSION 11
//#include <OVR_CAPI_D3D.h>  // Include SDK-rendered code for the D3D version
//
//#include "CKinect.h"
//
//#include <algorithm>
//#include <array>
//#include <memory>
//#include <stdexcept>
//#include <unordered_map>
//#include <vector>
//
//		 _COM_SMARTPTR_TYPEDEF(IDXGIFactory, __uuidof(IDXGIFactory));
//_COM_SMARTPTR_TYPEDEF(IDXGIAdapter, __uuidof(IDXGIAdapter));
//_COM_SMARTPTR_TYPEDEF(IDXGISwapChain, __uuidof(IDXGISwapChain));
//_COM_SMARTPTR_TYPEDEF(ID3D11Device, __uuidof(ID3D11Device));
//_COM_SMARTPTR_TYPEDEF(ID3D11DeviceContext, __uuidof(ID3D11DeviceContext));
//_COM_SMARTPTR_TYPEDEF(ID3D11Texture2D, __uuidof(ID3D11Texture2D));
//_COM_SMARTPTR_TYPEDEF(ID3D11RenderTargetView, __uuidof(ID3D11RenderTargetView));
//_COM_SMARTPTR_TYPEDEF(ID3D11ShaderResourceView, __uuidof(ID3D11ShaderResourceView));
//_COM_SMARTPTR_TYPEDEF(ID3D11DepthStencilView, __uuidof(ID3D11DepthStencilView));
//_COM_SMARTPTR_TYPEDEF(ID3D11Buffer, __uuidof(ID3D11Buffer));
//_COM_SMARTPTR_TYPEDEF(ID3D11RasterizerState, __uuidof(ID3D11RasterizerState));
//_COM_SMARTPTR_TYPEDEF(ID3D11DepthStencilState, __uuidof(ID3D11DepthStencilState));
//_COM_SMARTPTR_TYPEDEF(ID3D11VertexShader, __uuidof(ID3D11VertexShader));
//_COM_SMARTPTR_TYPEDEF(ID3D11PixelShader, __uuidof(ID3D11PixelShader));
//_COM_SMARTPTR_TYPEDEF(ID3D11ShaderReflection, __uuidof(ID3D11ShaderReflection));
//_COM_SMARTPTR_TYPEDEF(ID3D11InputLayout, __uuidof(ID3D11InputLayout));
//_COM_SMARTPTR_TYPEDEF(ID3D11SamplerState, __uuidof(ID3D11SamplerState));
//_COM_SMARTPTR_TYPEDEF(ID3DBlob, __uuidof(ID3DBlob));
//
//using namespace OVR;
//using namespace std;
//
//struct EyeTarget {
//	ID3D11Texture2DPtr tex;
//	ID3D11ShaderResourceViewPtr srv;
//	ID3D11RenderTargetViewPtr rtv;
//	ID3D11DepthStencilViewPtr dsv;
//	ovrRecti viewport;
//	Sizei size;
//
//	EyeTarget(ID3D11Device* device, Sizei size);
//};
//
//struct DirectX11 {
//	HINSTANCE hinst = nullptr;
//	HWND window = nullptr;
//	array<bool, 256> keys;
//	ID3D11DevicePtr device;
//	ID3D11DeviceContextPtr context;
//	IDXGISwapChainPtr swapChain;
//	ID3D11RenderTargetViewPtr backBufferRT;
//	ID3D11BufferPtr uniformBufferGen;
//	ID3D11SamplerStatePtr samplerState;
//	ID3D11VertexShaderPtr vShader;
//	vector<unsigned char> uniformData;
//	unordered_map<string, int> uniformOffsets;
//	ID3D11PixelShaderPtr pShader;
//	ID3D11InputLayoutPtr inputLayout;
//
//	DirectX11(HINSTANCE hinst, const Recti& vp);
//	~DirectX11();
//	void ClearAndSetEyeTarget(const EyeTarget& eyeTarget);
//	void UpdateSubResource(ID3D11Buffer* vertices, const void *pSrcData, UINT SrcRowPitch);
//	void Render(ID3D11Buffer* vertices, ID3D11Buffer* indices,
//		UINT stride, int count);
//	bool IsAnyKeyPressed() const;
//	void SetUniform(const char* name, int n, const float* v);
//};
//
//
//
//void throwOnError(ovrBool res, ovrHmd hmd = nullptr) {
//	if (!res) {
//		auto errString = ovrHmd_GetLastError(hmd);
//#ifdef _DEBUG
//		OutputDebugStringA(errString);
//#endif
//		throw runtime_error{ errString };
//	}
//}
//
//template <typename Func>
//struct scope_exit {
//	scope_exit(Func f) : onExit(f) {}
//	~scope_exit() { onExit(); }
//
//private:
//	Func onExit;
//};
//
//template <typename Func>
//scope_exit<Func> on_scope_exit(Func f) {
//	return scope_exit<Func>{f};
//};
//
////-------------------------------------------------------------------------------------
//int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR /*args*/, int) {
//	//initizalize kinect
//	InitializeDefaultSensor();
//
//	// Initialize the OVR SDK
//	throwOnError(ovr_Initialize());
//	auto ovr = on_scope_exit([] { ovr_Shutdown(); });
//
//	// Create the HMD
//	auto hmdCreate = [] {
//		auto hmd = ovrHmd_Create(0);
//		if (!hmd) {
//			MessageBoxA(NULL, "Oculus Rift not detected.\nAttempting to create debug HMD.", "",
//				MB_OK);
//
//			// If we didn't detect an Hmd, create a simulated one for debugging.
//			hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
//			throwOnError(hmd != nullptr);
//		}
//
//		if (hmd->ProductName[0] == '\0')
//			MessageBoxA(NULL, "Rift detected, display not enabled.", "", MB_OK);
//		return hmd;
//	};
//	auto hmdDestroy = [](ovrHmd hmd) { ovrHmd_Destroy(hmd); };
//	unique_ptr<const ovrHmdDesc, decltype(hmdDestroy)> hmd{ hmdCreate(), hmdDestroy };
//
//	// Create the Direct3D11 device and window
//	DirectX11 dx11{ hinst, Recti{ hmd->WindowsPos, hmd->Resolution } };
//
//	// Attach HMD to window and initialize tracking
//	throwOnError(ovrHmd_AttachToWindow(hmd.get(), dx11.window, nullptr, nullptr), hmd.get());
//	ovrHmd_SetEnabledCaps(hmd.get(), ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);
//	throwOnError(ovrHmd_ConfigureTracking(hmd.get(), ovrTrackingCap_Orientation |
//		ovrTrackingCap_MagYawCorrection |
//		ovrTrackingCap_Position,
//		0),
//		hmd.get());
//
//	// Create the eye render targets.
//	const EyeTarget eyeTargets[] = {
//			{ dx11.device,
//			ovrHmd_GetFovTextureSize(hmd.get(), ovrEye_Left, hmd->DefaultEyeFov[ovrEye_Left], 1.0f) },
//			{ dx11.device, ovrHmd_GetFovTextureSize(hmd.get(), ovrEye_Right,
//			hmd->DefaultEyeFov[ovrEye_Right], 1.0f) } };
//
//	// Configure SDK rendering
//	auto eyeRenderDesc = [&dx11, &hmd] {
//		ovrD3D11Config d3d11cfg{};
//		d3d11cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
//		d3d11cfg.D3D11.Header.BackBufferSize = hmd->Resolution;
//		d3d11cfg.D3D11.Header.Multisample = 1;
//		d3d11cfg.D3D11.pDevice = dx11.device;
//		d3d11cfg.D3D11.pDeviceContext = dx11.context;
//		d3d11cfg.D3D11.pBackBufferRT = dx11.backBufferRT;
//		d3d11cfg.D3D11.pSwapChain = dx11.swapChain;
//
//		array<ovrEyeRenderDesc, 2> res;
//		throwOnError(
//			ovrHmd_ConfigureRendering(hmd.get(), &d3d11cfg.Config,
//			ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette |
//			ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive,
//			hmd->DefaultEyeFov, &res[0]),
//			hmd.get());
//		return res;
//	}();
//
//
//	float yaw = 0.541592f;            // Horizontal rotation of the player
//	Vector3f pos{ 0.0f, 0.0f, 0.0f };  // Position of player
//
//	//kinect buffers
//	ID3D11Buffer*           kinectVertexBuffer = nullptr;
//	ID3D11Buffer*           kinectIndexBuffer = nullptr;
//
//	//allocate vertex buffer
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = (sizeof(CameraSpacePoint) * cDepthWidth * cDepthHeight);
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	//bd.StructureByteStride = 12;
//	bd.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = kinectVerticies;
//	HRESULT hr = dx11.device->CreateBuffer(&bd, &InitData, &kinectVertexBuffer);
//	if (FAILED(hr))
//		return hr;
//
//
//
//	//fill and allocate index buffer
//	DWORD * indices;
//	indices = new DWORD[(cDepthWidth * cDepthHeight * 6)];
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
//	//allocate index buffer
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = cDepthWidth * cDepthHeight * 6 * sizeof(DWORD);  
//	bd.StructureByteStride = 4;
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	InitData.pSysMem = indices;
//	hr = dx11.device->CreateBuffer(&bd, &InitData, &kinectIndexBuffer);
//	if (FAILED(hr))
//		return hr;
//	
//
//
//	// MAIN LOOP
//	// =========
//	int appClock = 0;
//
//	while (!(dx11.keys['Q'] && dx11.keys[VK_CONTROL]) && !dx11.keys[VK_ESCAPE]) {
//		++appClock;
//
//		//kinect
//		update();//get most recent kinect data, stores in kinectVerticies in CKinect.h
//		//update kinect subresource
//		dx11.context->UpdateSubresource(kinectVertexBuffer, 0, 0, kinectVerticies, sizeof(kinectVerticies), 0);
//
//		MSG msg{};
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//
//		const float speed = 1.0f;  // Can adjust the movement speed.
//		ovrVector3f useHmdToEyeViewOffset[2] = { eyeRenderDesc[0].HmdToEyeViewOffset,
//			eyeRenderDesc[1].HmdToEyeViewOffset };
//
//		ovrHmd_BeginFrame(hmd.get(), 0);
//
//		// Recenter the Rift by pressing 'R'
//		if (dx11.keys['R']) ovrHmd_RecenterPose(hmd.get());
//
//		// Dismiss the Health and Safety message by pressing any key
//		if (dx11.IsAnyKeyPressed()) ovrHmd_DismissHSWDisplay(hmd.get());
//
//		// Keyboard inputs to adjust player orientation
//		if (dx11.keys[VK_LEFT]) yaw += 0.02f;
//		if (dx11.keys[VK_RIGHT]) yaw -= 0.02f;
//
//		// Keyboard inputs to adjust player position
//		if (dx11.keys['W'] || dx11.keys[VK_UP])
//			pos += Matrix4f::RotationY(yaw).Transform(Vector3f(0, 0, -speed * 0.05f));
//		if (dx11.keys['S'] || dx11.keys[VK_DOWN])
//			pos += Matrix4f::RotationY(yaw).Transform(Vector3f(0, 0, +speed * 0.05f));
//		if (dx11.keys['D'])
//			pos += Matrix4f::RotationY(yaw).Transform(Vector3f(+speed * 0.05f, 0, 0));
//		if (dx11.keys['A'])
//			pos += Matrix4f::RotationY(yaw).Transform(Vector3f(-speed * 0.05f, 0, 0));
//		pos.y = ovrHmd_GetFloat(hmd.get(), OVR_KEY_EYE_HEIGHT, pos.y);
//
//
//		// Get both eye poses simultaneously, with IPD offset already included.
//		ovrPosef eyePoses[2] = {};
//		ovrHmd_GetEyePoses(hmd.get(), 0, useHmdToEyeViewOffset, eyePoses, nullptr);
//
//		// Render the two undistorted eye views into their render buffers.
//		for (int eye = 0; eye < 2; ++eye) {
//			const auto& useTarget = eyeTargets[eye];
//			const auto& useEyePose = eyePoses[eye];
//
//			dx11.ClearAndSetEyeTarget(useTarget);
//
//			// Get view and projection matrices (note near Z to reduce eye strain)
//			const Matrix4f rollPitchYaw = Matrix4f::RotationY(yaw);
//			const Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(useEyePose.Orientation);
//			const Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f{ 0, 1, 0 });
//			const Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f{ 0, 0, -1 });
//			const Vector3f shiftedEyePos = pos + rollPitchYaw.Transform(useEyePose.Position);
//
//			const Matrix4f view =
//				Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
//			const Matrix4f proj =
//				ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.2f, 1000.0f, true);
//
//
//			// Render the scene
//			dx11.SetUniform("Proj", 16, &proj.Transposed().M[0][0]);
//			dx11.SetUniform("View", 16, &view.Transposed().M[0][0]);
//			dx11.Render(kinectVertexBuffer, kinectIndexBuffer, sizeof(CameraSpacePoint), cDepthWidth * cDepthHeight);
//
//		}
//
//		// Do distortion rendering, Present and flush/sync
//		[&eyeTargets, &eyePoses, &hmd] {
//			ovrD3D11Texture eyeTexture[2];
//			for (int eye = 0; eye < 2; ++eye) {
//				eyeTexture[eye].D3D11.Header.API = ovrRenderAPI_D3D11;
//				eyeTexture[eye].D3D11.Header.TextureSize = eyeTargets[eye].size;
//				eyeTexture[eye].D3D11.Header.RenderViewport = eyeTargets[eye].viewport;
//				eyeTexture[eye].D3D11.pTexture = eyeTargets[eye].tex;
//				eyeTexture[eye].D3D11.pSRView = eyeTargets[eye].srv;
//			}
//			ovrHmd_EndFrame(hmd.get(), eyePoses, &eyeTexture[0].Texture);
//		}();
//	}
//
//	return 0;
//}
//
//void ThrowOnFailure(HRESULT hr) {
//	if (FAILED(hr)) {
//		_com_error err{ hr };
//		OutputDebugString(err.ErrorMessage());
//		throw runtime_error{ "Failed HRESULT" };
//	}
//}
//
//EyeTarget::EyeTarget(ID3D11Device* device, Sizei requestedSize) {
//	CD3D11_TEXTURE2D_DESC texDesc(DXGI_FORMAT_R8G8B8A8_UNORM, requestedSize.w, requestedSize.h);
//	texDesc.MipLevels = 1;
//	texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
//	device->CreateTexture2D(&texDesc, nullptr, &tex);
//	device->CreateShaderResourceView(tex, nullptr, &srv);
//	device->CreateRenderTargetView(tex, nullptr, &rtv);
//	tex->GetDesc(&texDesc);  // Get the actual size in case it was adjusted on create
//	size = Sizei(texDesc.Width, texDesc.Height);
//
//	CD3D11_TEXTURE2D_DESC dsDesc{ DXGI_FORMAT_D32_FLOAT, texDesc.Width, texDesc.Height };
//	dsDesc.MipLevels = 1;
//	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	ID3D11Texture2DPtr dsTex;
//	device->CreateTexture2D(&dsDesc, nullptr, &dsTex);
//	device->CreateDepthStencilView(dsTex, nullptr, &dsv);
//
//	viewport.Pos = Vector2i{ 0, 0 };
//	viewport.Size = Sizei(texDesc.Width, texDesc.Height);
//}
//
//LRESULT CALLBACK SystemWindowProc(HWND arg_hwnd, UINT msg, WPARAM wp, LPARAM lp) {
//	static DirectX11* dx11 = nullptr;
//
//	switch (msg) {
//	case (WM_NCCREATE) : {
//		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lp);
//		if (createStruct->lpCreateParams) {
//			dx11 = reinterpret_cast<DirectX11*>(createStruct->lpCreateParams);
//			dx11->window = arg_hwnd;
//		}
//		break;
//	}
//	case WM_KEYDOWN:
//		if (dx11) dx11->keys[(unsigned)wp] = true;
//		break;
//	case WM_KEYUP:
//		if (dx11) dx11->keys[(unsigned)wp] = false;
//		break;
//	case WM_SETFOCUS:
//		SetCapture(dx11->window);
//		//ShowCursor(FALSE);
//		break;
//	case WM_KILLFOCUS:
//		ReleaseCapture();
//		ShowCursor(TRUE);
//		break;
//	}
//	return DefWindowProc(arg_hwnd, msg, wp, lp);
//}
//
//DirectX11::DirectX11(HINSTANCE hinst_, const Recti& vp) : hinst{ hinst_ } {
//	fill(begin(keys), end(keys), false);
//
//	window = [this, vp] {
//		const auto className = L"OVRAppWindow";
//		WNDCLASSW wc{};
//		wc.lpszClassName = className;
//		wc.lpfnWndProc = SystemWindowProc;
//		RegisterClassW(&wc);
//
//		const DWORD wsStyle = WS_POPUP | WS_OVERLAPPEDWINDOW;
//		const auto sizeDivisor = 2;
//		RECT winSize{ 0, 0, vp.w / sizeDivisor, vp.h / sizeDivisor };
//		AdjustWindowRect(&winSize, wsStyle, false);
//		return CreateWindowW(className, L"Corpus Maximus", wsStyle | WS_VISIBLE, vp.x, vp.y,
//			winSize.right - winSize.left, winSize.bottom - winSize.top, nullptr,
//			nullptr, hinst, this);
//	}();
//
//	[vp](HWND hwnd, IDXGISwapChain** sc, ID3D11Device** dev, ID3D11DeviceContext** ctx) {
//		IDXGIFactoryPtr DXGIFactory;
//		ThrowOnFailure(
//			CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&DXGIFactory)));
//
//		IDXGIAdapterPtr Adapter;
//		ThrowOnFailure(DXGIFactory->EnumAdapters(0, &Adapter));
//
//		const UINT creationFlags =
//#ifdef _DEBUG
//			D3D11_CREATE_DEVICE_DEBUG;
//#else
//			0u;
//#endif
//
//		DXGI_SWAP_CHAIN_DESC scDesc{};
//		scDesc.BufferCount = 2;
//		scDesc.BufferDesc.Width = vp.GetSize().w;
//		scDesc.BufferDesc.Height = vp.GetSize().h;
//		scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//		scDesc.OutputWindow = hwnd;
//		scDesc.SampleDesc.Count = 1;
//		scDesc.SampleDesc.Quality = 0;
//		scDesc.Windowed = TRUE;
//		scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//
//		ThrowOnFailure(D3D11CreateDeviceAndSwapChain(
//			Adapter, Adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE, nullptr,
//			creationFlags, nullptr, 0, D3D11_SDK_VERSION, &scDesc, sc, dev, nullptr, ctx));
//	}(window, &swapChain, &device, &context);
//
//	[](IDXGISwapChain* sc, ID3D11Device* dev, ID3D11RenderTargetView** backBufferRtv) {
//		ID3D11Texture2DPtr backBuffer;
//		ThrowOnFailure(
//			sc->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
//		ThrowOnFailure(dev->CreateRenderTargetView(backBuffer, nullptr, backBufferRtv));
//	}(swapChain, device, &backBufferRT);
//
//	[](ID3D11Device* dev, ID3D11Buffer** uniformBuffer) {
//		CD3D11_BUFFER_DESC desc{ 2000u, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC,
//			D3D11_CPU_ACCESS_WRITE };
//		ThrowOnFailure(dev->CreateBuffer(&desc, nullptr, uniformBuffer));
//	}(device, &uniformBufferGen);
//
//	[](ID3D11Device* dev, ID3D11DeviceContext* ctx) {
//		CD3D11_RASTERIZER_DESC desc{ D3D11_DEFAULT };
//		ID3D11RasterizerStatePtr rasterizerState;
//		ThrowOnFailure(dev->CreateRasterizerState(&desc, &rasterizerState));
//		ctx->RSSetState(rasterizerState);
//	}(device, context);
//
//	[](ID3D11Device* dev, ID3D11DeviceContext* ctx) {
//		CD3D11_DEPTH_STENCIL_DESC desc{ D3D11_DEFAULT };
//		ID3D11DepthStencilStatePtr depthStencilState;
//		ThrowOnFailure(dev->CreateDepthStencilState(&desc, &depthStencilState));
//		ctx->OMSetDepthStencilState(depthStencilState, 0);
//	}(device, context);
//
//	[](ID3D11Device* dev, ID3D11SamplerState** ss) {
//		CD3D11_SAMPLER_DESC desc{ D3D11_DEFAULT };
//		desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//		desc.Filter = D3D11_FILTER_ANISOTROPIC;
//		desc.MaxAnisotropy = 8;
//		dev->CreateSamplerState(&desc, ss);
//	}(device, &samplerState);
//
//	[this](ID3D11Device* dev, ID3D11VertexShader** vertexShader, ID3D11InputLayout** il) {
//		D3D11_INPUT_ELEMENT_DESC desc[] = {
//				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//		};
//
//		const char* VertexShaderSrc = R"(
//        float4x4 Proj, View;
//        void main(in float4 Position : POSITION, 
//                  out float4 oPosition : SV_Position )
//        {
//            float4 wp = Position;
//            oPosition = mul(Proj, mul(View, wp));
//        })";
//
//		ID3DBlobPtr blobData;
//		ThrowOnFailure(D3DCompile(VertexShaderSrc, strlen(VertexShaderSrc), nullptr, nullptr,
//			nullptr, "main", "vs_4_0", 0, 0, &blobData, nullptr));
//
//		ThrowOnFailure(dev->CreateVertexShader(blobData->GetBufferPointer(),
//			blobData->GetBufferSize(), NULL, vertexShader));
//
//		ID3D11ShaderReflectionPtr ref;
//		D3DReflect(blobData->GetBufferPointer(), blobData->GetBufferSize(),
//			__uuidof(ID3D11ShaderReflection), reinterpret_cast<void**>(&ref));
//		ID3D11ShaderReflectionConstantBuffer* buf = ref->GetConstantBufferByIndex(0);
//		D3D11_SHADER_BUFFER_DESC bufd{};
//		ThrowOnFailure(buf->GetDesc(&bufd));
//
//		for (unsigned i = 0; i < bufd.Variables; ++i) {
//			ID3D11ShaderReflectionVariable* var = buf->GetVariableByIndex(i);
//			D3D11_SHADER_VARIABLE_DESC vd{};
//			var->GetDesc(&vd);
//			uniformOffsets[vd.Name] = vd.StartOffset;
//		}
//		uniformData.resize(bufd.Size);
//
//		device->CreateInputLayout(desc, 1, blobData->GetBufferPointer(), blobData->GetBufferSize(), il);
//	}(device, &vShader, &inputLayout);
//
//	[](ID3D11Device* dev, ID3D11PixelShader** pixelShader) {
//		const char* PixelShaderSrc = R"(
//        float4 main(in float4 Position : SV_Position) : SV_Target
//        {
//            return float4(0.5, 0.5, 0.5, 0.5);
//        })";
//
//		ID3DBlobPtr blobData;
//		ThrowOnFailure(D3DCompile(PixelShaderSrc, strlen(PixelShaderSrc), nullptr, nullptr, nullptr,
//			"main", "ps_4_0", 0, 0, &blobData, nullptr));
//		ThrowOnFailure(dev->CreatePixelShader(blobData->GetBufferPointer(),
//			blobData->GetBufferSize(), nullptr, pixelShader));
//	}(device, &pShader);
//}
//
//DirectX11::~DirectX11() {
//	DestroyWindow(window);
//	UnregisterClassW(L"OVRAppWindow", hinst);
//}
//
//void DirectX11::ClearAndSetEyeTarget(const EyeTarget& eyeTarget) {
//	const float black[] = { 0.f, 0.f, 0.f, 1.f };
//	ID3D11RenderTargetView* rtvs[] = { eyeTarget.rtv };
//	context->OMSetRenderTargets(1, rtvs, eyeTarget.dsv);
//	context->ClearRenderTargetView(eyeTarget.rtv, black);
//	context->ClearDepthStencilView(eyeTarget.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
//	D3D11_VIEWPORT d3dvp{};
//	d3dvp.TopLeftX = static_cast<float>(eyeTarget.viewport.Pos.x);
//	d3dvp.TopLeftY = static_cast<float>(eyeTarget.viewport.Pos.y);
//	d3dvp.Width = static_cast<float>(eyeTarget.viewport.Size.w);
//	d3dvp.Height = static_cast<float>(eyeTarget.viewport.Size.h);
//	d3dvp.MinDepth = 0.f;
//	d3dvp.MaxDepth = 1.f;
//	context->RSSetViewports(1, &d3dvp);
//}
//
//
//void DirectX11::Render( ID3D11Buffer* vertices,
//	ID3D11Buffer* indices, UINT stride, int count) {
//
//	context->IASetInputLayout(inputLayout);
//	context->IASetIndexBuffer(indices, DXGI_FORMAT_R32_UINT, 0);
//
//	UINT offset = 0;
//	ID3D11Buffer* vertexBuffers[] = { vertices };
//	context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
//
//	D3D11_MAPPED_SUBRESOURCE map;
//	context->Map(uniformBufferGen, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
//	memcpy(map.pData, uniformData.data(), uniformData.size());
//	context->Unmap(uniformBufferGen, 0);
//
//	ID3D11Buffer* vsConstantBuffers[] = { uniformBufferGen };
//	context->VSSetConstantBuffers(0, 1, vsConstantBuffers);
//
//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	context->VSSetShader(vShader, nullptr, 0);
//	context->PSSetShader(pShader, nullptr, 0);
//
//	context->DrawIndexed(count, 0, 0);
//}
//
//bool DirectX11::IsAnyKeyPressed() const {
//	return any_of(begin(keys), end(keys), [](bool b) { return b; });
//}
//
//void DirectX11::SetUniform(const char* name, int n, const float* v) {
//	memcpy(uniformData.data() + uniformOffsets[name], v, n * sizeof(float));
//}
//
//
//
//
//
//
//
