#include "GameApplication.h"

//structure to hold a vertex
struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXCOLOR colour;
	D3DXVECTOR2 texCoords;
};

//Creates a Class - Constructor
CGameApplication::CGameApplication(void)
{
	m_pWindow=NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
	m_pVertexBuffer=NULL;
	m_pDepthStencilView=NULL;
	m_pDepthStencilTexture=NULL;
	m_pVertexLayout=NULL;
	m_pEffect=NULL;
	m_pTechnique=NULL;
	m_pWorldMatrixVariable=NULL;
	m_pViewMatrixVariable=NULL;
	m_pProjectionMatrixVariable=NULL;
}

//Destroys a class - Deconstructor
CGameApplication::~CGameApplication(void)
{
	if(m_pD3D10Device)
		m_pD3D10Device->ClearState();

	if(m_pVertexBuffer) //reclaim all memory allocated to buffer object
		m_pVertexBuffer->Release();

	if(m_pVertexLayout) //release the input layout
		m_pVertexLayout->Release();

	if(m_pEffect) //dealocate memory for created effects
		m_pEffect->Release();

	if(m_pIndexBuffer)
		m_pIndexBuffer->Release();

	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();

	if(m_pDepthStencilTexture)
		m_pDepthStencilTexture->Release();

	if(m_pDepthStencilView)
		m_pDepthStencilView->Release();

	if(m_pSwapChain)
		m_pSwapChain->Release();

	if(m_pD3D10Device)
		m_pD3D10Device->Release();

	if(m_pWindow)
	{
		delete m_pWindow;
		m_pWindow=NULL;
	}
}

bool CGameApplication::init()
{
	if(!initWindow())
		return false;

	if(!initGraphics())
		return false;

	if(!initGame())
		return false;

	return true;
}

//creates objects used to run the game, textures, effects and 3d models
bool CGameApplication::initGame()
{
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS; //puts effect loading in debug mode which gives us more information
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	ID3D10Blob *pErrors = NULL;

	if(FAILED(D3DX10CreateEffectFromFile(TEXT("Texture.fx"),
		NULL,NULL,"fx_4_0",dwShaderFlags,0,
		m_pD3D10Device,NULL,NULL,&m_pEffect,
		&pErrors,NULL)))
	{
		MessageBoxA(NULL,(char*)pErrors->GetBufferPointer(),
			"Error",
			MB_OK);
		return false;
	}

	m_pTechnique = m_pEffect->GetTechniqueByName("Render"); //pass in string which is the name of the effect we want to call

	//Create Vertex Buffer
	D3D10_BUFFER_DESC bd; //used to specify options for when we create a buffer
	bd.Usage = D3D10_USAGE_DEFAULT; //describes how the buffer is read/written to
	bd.ByteWidth = sizeof(Vertex)*4; //the size of the buffer
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER; //type of buffer we are creating
	bd.CPUAccessFlags = 0; //to specify if the buffer can read/written to by CPU
	bd.MiscFlags = 0; // for additional options

	Vertex vertices[] =
	{
		{D3DXVECTOR3(0.0f,0.5f,0.0f),D3DXCOLOR(0.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f)},
		{D3DXVECTOR3(0.5f,0.0f,0.0f),D3DXCOLOR(1.0f,0.0f,1.0f,1.0f),D3DXVECTOR2(1.0f,1.0f)},
	    {D3DXVECTOR3(0.5f,0.5f,0.0f),D3DXCOLOR(1.0f,1.0f,0.0f,1.0f),D3DXVECTOR2(0.0f,1.0f)},
        {D3DXVECTOR3(0.0f,0.0f,0.0f),D3DXCOLOR(1.0f,0.5f,1.0f,1.0f),D3DXVECTOR2(1.0f,0.0f)},
	};

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	if(FAILED(m_pD3D10Device->CreateBuffer(&bd,&InitData,&m_pVertexBuffer)))
		return false;

	int indices[]={0,1,2,0,1,3};

	//Create Index Buffer
	D3D10_BUFFER_DESC indexBufferDesc; //used to specify options for when we create a buffer
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT; //describes how the buffer is read/written to
	indexBufferDesc.ByteWidth = sizeof(indices)*6; //the size of the buffer
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER; //type of buffer we are creating
	indexBufferDesc.CPUAccessFlags = 0; //to specify if the buffer can read/written to by CPU
	indexBufferDesc.MiscFlags = 0; // for additional options
	
	D3D10_SUBRESOURCE_DATA IndexBufferInitialData;
	IndexBufferInitialData.pSysMem = indices;

	if(FAILED(m_pD3D10Device->CreateBuffer(&indexBufferDesc,&IndexBufferInitialData,&m_pIndexBuffer)))
		return false;

	m_pD3D10Device->IASetIndexBuffer(m_pIndexBuffer,DXGI_FORMAT_R32_UINT,0);

	D3D10_INPUT_ELEMENT_DESC layout[] = //Layout descriptions for different elements of vertex
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D10_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D10_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,28,D3D10_INPUT_PER_VERTEX_DATA,0},
	};

	UINT numElements = sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC); //calculates the number of elements in the input array
	D3D10_PASS_DESC PassDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc); // recives the pass description from the technique

	//creates input layout
	if(FAILED(m_pD3D10Device->CreateInputLayout(layout,
		numElements,
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&m_pVertexLayout)))
	{
		return false;
	}

	m_pD3D10Device->IASetInputLayout(m_pVertexLayout); //tells input assembler about input layout

	UINT stride = sizeof(Vertex); // assigns a value to hold the size of one vertex
	UINT offset = 0; //assigns a value to hold the offset which is where the vertices start in the vertex buffer
	

	m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //tells Input Assembler what kind of primitive to draw
	
	m_pD3D10Device->IASetVertexBuffers(0,1,&m_pVertexBuffer,&stride,&offset); //binds one or many buffers to the input assembler

	//Calculates the camera position and where it is looking
	D3DXVECTOR3 cameraPos(0.0f,0.0f,-5.0f);
	D3DXVECTOR3 cameraLook(0.0f,0.0f,1.0f);
	D3DXVECTOR3 cameraUp(0.0f,1.0f,0.0f);
	D3DXMatrixLookAtLH(&m_matView,&cameraPos,
		&cameraLook,&cameraUp);

	D3D10_VIEWPORT vp; //Gets first vieport that holds dimensions for our screen
	UINT numViewPorts=1;
	m_pD3D10Device->RSGetViewports(&numViewPorts,&vp); //creates projection matrix

	D3DXMatrixPerspectiveFovLH(&m_matProjection,(float)D3DX_PI * 0.25f,
		vp.Width/(FLOAT)vp.Height,0.1f,100.0f); //creates projection matrix

	m_pViewMatrixVariable = m_pEffect->GetVariableByName("matView")->AsMatrix();
	m_pProjectionMatrixVariable = m_pEffect->GetVariableByName("matProjection")->AsMatrix();

	m_pProjectionMatrixVariable->SetMatrix((float*)m_matProjection);

	m_vecPosition = D3DXVECTOR3(0.0f,0.0f,0.0f); //sets position of vectors
	m_vecScale = D3DXVECTOR3(1.0f,1.0f,1.0f); //sets scale of vectors
	m_vecRotation = D3DXVECTOR3(0.0f,0.0f,0.0f); //sets rotation of vectors
	m_pWorldMatrixVariable = m_pEffect->GetVariableByName("matWorld")->AsMatrix(); //retrieves world matrix from effect andsends world matrix to effect

	return true; 
}

//Intialization code for the game
bool CGameApplication::run()
{
	while(m_pWindow->running())
	{
		if(! m_pWindow->checkForWindowMessages())
		{
			update();
			render();
		}
	}
	return false;
}

//Loops until window closes, used to check window messages, update and render
void CGameApplication::render()
{
	float ClearColor[4] = {0.0f,0.125f,0.3f,1.0f}; //sets up float array for RGBA
	m_pD3D10Device->ClearRenderTargetView(m_pRenderTargetView,ClearColor); //will clear the render target to the above colour 

	m_pD3D10Device->ClearDepthStencilView(m_pDepthStencilView,D3D10_CLEAR_DEPTH,1.0f,0);

	m_pViewMatrixVariable->SetMatrix((float*)m_matView); //sent the view matrix to the effect

	m_pWorldMatrixVariable->SetMatrix((float*)m_matWorld);

	D3D10_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for(UINT p=0; p<techDesc.Passes;++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0);
		m_pD3D10Device->DrawIndexed(6,0,0);
	}

	m_pSwapChain->Present(0,0); //flips swap chain so back buffer is copied to front buffer
}

//Draw Code
void CGameApplication::update()
{
	D3DXMatrixScaling(&m_matScale,m_vecScale.x,m_vecScale.y,m_vecScale.z);

	D3DXMatrixRotationYawPitchRoll(&m_matRotation,m_vecRotation.y,
		m_vecRotation.x,m_vecRotation.z);

	D3DXMatrixTranslation(&m_matTranslation,m_vecPosition.x,
		m_vecPosition.y,m_vecPosition.z);

	D3DXMatrixMultiply(&m_matWorld,&m_matScale,&m_matRotation);
	D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&m_matTranslation);
}

//Update game state, AI, input devices and physics
bool CGameApplication::initGraphics()
{
	//retrieves the width and height of the window
	RECT windowRect;
	GetClientRect(m_pWindow->getHandleToWindow(),&windowRect);

	//store these values in unsigned int's
	UINT width=windowRect.right-windowRect.left;
	UINT height=windowRect.bottom-windowRect.top;

	UINT createDeviceFlags=0;
#ifdef _DEBUG
	createDeviceFlags|=D3D10_CREATE_DEVICE_DEBUG;
#endif

	//Holds all information for the creation of the swapchain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd,sizeof(sd));

	//checks if the window is fullscreen then specifys buffers
	if(m_pWindow->isFullScreen())
		sd.BufferCount = 2;
	else
		sd.BufferCount = 1;

	//associates a window handle with swap chain description
	sd.OutputWindow = m_pWindow->getHandleToWindow();
	sd.Windowed = (BOOL)(!m_pWindow->isFullScreen());
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.SampleDesc.Count = 1; //sets the Multisampling parameters for the swap chain
	sd.SampleDesc.Quality = 0;

	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //sets format of buffer, which has 8 bytes for RBGA
	sd.BufferDesc.RefreshRate.Numerator = 60; //sets refresh rate to 60Hz using an update of vertical blank
	sd.BufferDesc.RefreshRate.Denominator = 1;

	//create a swap chain and device in one call surrounded by if failed
	if(FAILED(D3D10CreateDeviceAndSwapChain(NULL,
		D3D10_DRIVER_TYPE_HARDWARE,NULL,createDeviceFlags,
		D3D10_SDK_VERSION,&sd, &m_pSwapChain,&m_pD3D10Device)))

	return false;

	//associate a buffer from the swap chain with Render Taget View
	ID3D10Texture2D *pBackBuffer;
	if(FAILED(m_pSwapChain->GetBuffer(0,__uuidof(ID3D10Texture2D),
		(void**)&pBackBuffer)))
		return false;

	//creates a render target view
	if(FAILED(m_pD3D10Device->CreateRenderTargetView(pBackBuffer, NULL,
		&m_pRenderTargetView)))
	{
		pBackBuffer->Release();
		return false;
	}

	pBackBuffer->Release();

	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	
	if(FAILED(m_pD3D10Device->CreateTexture2D(&descDepth,NULL,&m_pDepthStencilTexture)))
		return false;

	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	if(FAILED(m_pD3D10Device->CreateDepthStencilView(
		m_pDepthStencilTexture,&descDSV,&m_pDepthStencilView)))
		return false;

	//Binds an array of render targets to the output manager stage of pipeline
	m_pD3D10Device->OMSetRenderTargets(1,&m_pRenderTargetView,m_pDepthStencilView);

	//sets up an instance of vp the same height and width as the window
	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	//calls RSSetViewports function to set the viewport
	m_pD3D10Device->RSSetViewports(1,&vp);
	
	
	return true;
}

bool CGameApplication::initWindow()
{
	m_pWindow=new CWin32Window();
	if(!m_pWindow->init(TEXT("Lab 1 - Create Device"),800,640,false))
		return false;

	return true;
}