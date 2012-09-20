#pragma once

#include "Win32Window.h"
#include <D3D10.h>
#include <D3DX10.h>

class 
	CGameApplication
{
public:
	CGameApplication(void);
	~CGameApplication(void);

	bool init();
	bool run();

private:
	bool initGame();
	bool initGraphics();
	bool initWindow();

	void render();
	void update();

private:
	ID3D10Device * m_pD3D10Device;
	IDXGISwapChain * m_pSwapChain;
	ID3D10RenderTargetView * m_pRenderTargetView;

	ID3D10DepthStencilView * m_pDepthStencilView;
	ID3D10Texture2D * m_pDepthStencilTexture;

	CWin32Window * m_pWindow;

	ID3D10Buffer*	m_pVertexBuffer;
	ID3D10InputLayout*	m_pVertexLayout; //used to hold the input layout used to describe the vertex to the pipeline

	ID3D10Effect*	m_pEffect; //holds our loaded effect file
	ID3D10EffectTechnique*	m_pTechnique; //variable used in rendering process

	ID3D10EffectMatrixVariable * m_pWorldMatrixVariable; //sends world matrix across effect

	ID3D10EffectMatrixVariable * m_pViewMatrixVariable;
	ID3D10EffectMatrixVariable * m_pProjectionMatrixVariable;

	ID3D10EffectShaderResourceVariable * m_pDiffuseTextureVariable;
	ID3D10ShaderResourceView * m_pTextureShaderResource;

	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProjection;

	D3DXMATRIX m_matWorld; //world matrix
	D3DXMATRIX m_matScale; //world matrix scale
	D3DXMATRIX m_matRotation; //world matrix rotation
	D3DXMATRIX m_matTranslation; // world matrix translation

	//used to represent the matrices as they are more human readable
	D3DXVECTOR3 m_vecPosition; //vector used to represent the position
	D3DXVECTOR3 m_vecRotation; //vector used to represent the rotation
	D3DXVECTOR3 m_vecScale; //vector used to represent the scale
};