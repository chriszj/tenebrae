//=============================================================================
//
// 深さレンダリング処理 [renderer.h]
// Author : 
//
//=============================================================================
#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitDepthRenderer(ID3D11Device* device, HWND hwnd);
void UninitDepthRenderer(void);
void SetDepthRenderShaders(ID3D11DeviceContext* device);
void SetDepthRenderTarget(ID3D11DeviceContext* deviceContext);
void ClearDepthRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha);
