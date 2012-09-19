#include "GameApplication.h"

//structure to hold a vertex
struct Vertex
{
	D3DXVECTOR3 pos;
};

//Creates a Class
CGameApplication::CGameApplication(void)
{
	m_pWindow=NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
	m_pVertexBuffer=NULL;
}

//Destroys a class
CGameApplication::~CGameApplication(void)
{
	if(m_pD3D10Device)
		m_pD3D10Device->ClearState();

	if(m_pVertexBuffer) //reclaim all memory allocated to buffer object
		m_pVertexBuffer->Release();

	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();
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
	D3D10_BUFFER_DESC bd; //used to specify options for when we create a buffer
	bd.Usage = D3D10_USAGE_DEFAULT; //describes how the buffer is read/written to
	bd.ByteWidth = sizeof(Vertex)*3; //the size of the buffer
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER; //type of buffer we are creating
	bd.CPUAccessFlags = 0; //to specify if the buffer can read/written to by CPU
	bd.MiscFlags = 0; // for additional options

	Vertex vertices[] =
	{
		D3DXVECTOR3(0.0f,0.5f,0.5f),
		D3DXVECTOR3(0.5f,-0.5f,0.5f),
		D3DXVECTOR3(-0.5f,-0.5f,0.5f),
	};

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	if(FAILED(m_pD3D10Device->CreateBuffer(&bd,&InitData,&m_pVertexBuffer)))
		return false;

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

	//ALL DRAW CODE SHOULD GO HERE
	

	m_pSwapChain->Present(0,0);//flips swap chain so back buffer is copied to front buffer
}

//Draw Code
void CGameApplication::update()
{
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

	//Binds an array of render targets to the output manager stage of pipeline
	m_pD3D10Device->OMSetRenderTargets(1,&m_pRenderTargetView,NULL);

	//sets up an instance of vp the sam height and width as the window
	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	//calls RSSetViewports function to se the viewport
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