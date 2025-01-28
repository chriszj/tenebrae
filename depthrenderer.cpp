#include "main.h"
#include "depthrenderer.h"
#include "renderer.h"

#define SHADOWMAP_SCREEN_DEPTH 100.0f
#define SHADOWMAP_SCREEN_NEAR  1.0f
#define SHADOWMAP_WIDTH       (1024)
#define SHADOWMAP_HEIGHT      (1024)

static IDXGISwapChain* g_SwapChain = NULL;
static ID3D11RenderTargetView* g_RenderTargetView = NULL;
static ID3D11DepthStencilView* g_DepthStencilView = NULL;

static ID3D11VertexShader* g_VertexShader = NULL;
static ID3D11PixelShader* g_PixelShader = NULL;
static ID3D11InputLayout* g_VertexLayout = NULL;

static ID3D11DepthStencilState* g_DepthStateEnable;
static ID3D11DepthStencilState* g_DepthStateDisable;

static ID3D11Texture2D* g_renderTargetTexture;
static ID3D11RenderTargetView* g_renderTargetView;
static ID3D11ShaderResourceView* g_shaderResourceView;
static ID3D11Texture2D* g_depthStencilBuffer;
static ID3D11DepthStencilView* g_depthStencilView;
static D3D11_VIEWPORT g_viewport;
static XMMATRIX g_projectionMatrix;
static XMMATRIX g_orthoMatrix;
static int g_textureWidth, g_textureHeight;

static float g_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// 背景色

HRESULT InitRenderTarget(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear, int format);
void UninitRenderTarget(void);

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitDepthRenderer(ID3D11Device* device, HWND hwnd) {

	HRESULT hr = S_OK;

	// 頂点シェーダコンパイル・生成
	ID3DBlob* pErrorBlob;
	ID3DBlob* pVSBlob = NULL;
	DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

	// メインレンディングクラスからD3Dデヴァイスを集めます
	hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "VertexShaderForSM", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
	}

	device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShader);

	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	device->CreateInputLayout(layout,
		numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&g_VertexLayout);

	pVSBlob->Release();


	// ピクセルシェーダコンパイル・生成
	ID3DBlob* pPSBlob = NULL;
	hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "PixelShaderForSM", "ps_4_0", shFlag, 0, NULL, &pPSBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
	}

	device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_PixelShader);

	pPSBlob->Release();

	return InitRenderTarget(device, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_SCREEN_DEPTH, SHADOWMAP_SCREEN_NEAR, 1);

};

HRESULT InitRenderTarget(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear, int format) {

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	DXGI_FORMAT textureFormat;

	// Set the texture format.
	switch (format)
	{
		case 1:
		{
			textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		}
		default:
		{
			textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		}
	}

	// Store the width and height of the render texture.
	g_textureWidth = textureWidth;
	g_textureHeight = textureHeight;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = textureFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &g_renderTargetTexture);
	if (FAILED(result))
	{
		return S_FALSE;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(g_renderTargetTexture, &renderTargetViewDesc, &g_renderTargetView);
	if (FAILED(result))
	{
		return S_FALSE;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(g_renderTargetTexture, &shaderResourceViewDesc, &g_shaderResourceView);
	if (FAILED(result))
	{
		return S_FALSE;
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &g_depthStencilBuffer);
	if (FAILED(result))
	{
		return S_FALSE;
	}

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(g_depthStencilBuffer, &depthStencilViewDesc, &g_depthStencilView);
	if (FAILED(result))
	{
		return S_FALSE;
	}

	// Setup the viewport for rendering.
	g_viewport.Width = (float)textureWidth;
	g_viewport.Height = (float)textureHeight;
	g_viewport.MinDepth = 0.0f;
	g_viewport.MaxDepth = 1.0f;
	g_viewport.TopLeftX = 0;
	g_viewport.TopLeftY = 0;

	// Setup the projection matrix.
	g_projectionMatrix = XMMatrixPerspectiveFovLH((3.141592654f / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	// Create an orthographic projection matrix for 2D rendering.
	g_orthoMatrix = XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenDepth);

	return TRUE;

};

void UninitDepthRenderer(void) {

	if (g_VertexShader)			g_VertexShader->Release();
	if (g_PixelShader)			g_PixelShader->Release();

};

void UninitRenderTarget(void) {

	if (g_depthStencilView)
	{
		g_depthStencilView->Release();
		g_depthStencilView = 0;
	}

	if (g_depthStencilBuffer)
	{
		g_depthStencilBuffer->Release();
		g_depthStencilBuffer = 0;
	}

	if (g_shaderResourceView)
	{
		g_shaderResourceView->Release();
		g_shaderResourceView = 0;
	}

	if (g_renderTargetView)
	{
		g_renderTargetView->Release();
		g_renderTargetView = 0;
	}

	if (g_renderTargetTexture)
	{
		g_renderTargetTexture->Release();
		g_renderTargetTexture = 0;
	}

};

void SetDepthRenderShaders(ID3D11DeviceContext* deviceContext) {


	// 入力レイアウト設定
	deviceContext->IASetInputLayout(g_VertexLayout);

	// シェーダ設定
	deviceContext->VSSetShader(g_VertexShader, NULL, 0);
	deviceContext->PSSetShader(g_PixelShader, NULL, 0);
};

void SetDepthRenderTarget(ID3D11DeviceContext* deviceContext)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &g_renderTargetView, g_depthStencilView);

	// Set the viewport.
	deviceContext->RSSetViewports(1, &g_viewport);

	return;
};


void ClearDepthRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(g_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
};

ID3D11ShaderResourceView* GetDepthRendererShaderResourceView()
{
	return g_shaderResourceView;
}