#include "Utility.h"
#include "Window.h"

#ifdef _WIN32

#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
// Instead of d3dx11 -> https://blogs.msdn.microsoft.com/chuckw/2013/08/20/living-without-d3dx/

// USING THIS : http://www.directxtutorial.com/

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

IDXGISwapChain *swapchain;
ID3D11Device *device;
ID3D11DeviceContext *deviceContext;
ID3D11RenderTargetView *backbuffer;

ID3D11VertexShader *pVS;    // the vertex shader
ID3D11PixelShader *pPS;     // the pixel shader

ID3D11Buffer *pVBuffer;     // the vertex buffer
ID3D11InputLayout *pLayout;

struct VERTEX
{
	float x, y, z;
	DirectX::XMFLOAT4 color;

};

void InitD3D(HWND hWnd);
void CleanD3D();
void InitPipeline();
void InitGraphics();

void renderFrame()
{
	DirectX::XMFLOAT4 color(0.0f, 0.2f, 0.4f, 1.0f);

	deviceContext->ClearRenderTargetView(backbuffer, &color.x);

	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	deviceContext->Draw(3, 0);

	swapchain->Present(0, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

#else // Other OS's
int main(int argc, char** argv)
{
#endif

#ifdef _WIN32
	ApplicationContext::getInstance().setAppHandle(hInstance);
#endif

	deq::Window w = deq::Window(1280, 720, "Test", deq::WindowStyle::WINDOW_VISIBLE);
	
	InitD3D(w.getWindowHandle());
	InitPipeline();
	InitGraphics();

	while (w.isOpen())
	{
		deq::Event e;
		while (w.pollEvents(e))
		{
		}

		renderFrame();
	}

	CleanD3D();
	return 0;
}

void InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;									// windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&device,
		NULL,
		&deviceContext);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	deviceContext->OMSetRenderTargets(1, &backbuffer, NULL);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;

	deviceContext->RSSetViewports(1, &viewport);
}

void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    // close and release all existing COM objects
    pVS->Release();
    pPS->Release();
    swapchain->Release();
    backbuffer->Release();
    device->Release();
    deviceContext->Release();
}

void InitPipeline()
{
	// load and compile the two shaders
	ID3D10Blob *VS, *PS;
	D3DCompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, &PS, 0);

	// encapsulate both shaders into shader objects
	device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	deviceContext->VSSetShader(pVS, 0, 0);
	deviceContext->PSSetShader(pPS, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	deviceContext->IASetInputLayout(pLayout);
}

void InitGraphics()
{
	// create a triangle using the VERTEX struct
	VERTEX OurVertices[] =
	{
		{ 0.0f, 0.5f, 0.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.45f, -0.5, 0.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ -0.45f, -0.5f, 0.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
	};


	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	device->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer


												   // copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	deviceContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	deviceContext->Unmap(pVBuffer, NULL);                                      // unmap the buffer

}