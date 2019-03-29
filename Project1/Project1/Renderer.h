//#pragma once
//
//#include <d3d11.h>
//#include <DirectXMath.h>
//
//
//class Renderer
//{
//public:
//	Renderer(HWND hWnd);
//
//	bool InitDevice(const int &width, const int &height);
//	bool CreateRenderTarget(const int &width, const int &height);
//	bool CompileShaderFromFile(const WCHAR* const szFileName, const LPCSTR const szEntryPoint, const LPCSTR const szShaderModel, ID3DBlob** const ppBlobOut) const;
//	void BeginFrame();
//	void EndFrame() const;
//	void Update();
//	void ShutDown();
//
//	inline ID3D11Device		*GetDevice() const
//	{
//		return m_pDevice;
//	}
//
//	inline ID3D11DeviceContext *GetDeviceContext() const
//	{
//		return m_pDeviceContext;
//	}
//
//	~Renderer();
//
//private:
//
//	ID3D11Device		*m_pDevice;
//	IDXGISwapChain		*m_pSwapChain;
//	ID3D11DeviceContext	*m_pDeviceContext;
//
//	ID3D11Texture2D		*m_pDepthStencil;
//	ID3D11DepthStencilView *m_pDepthStencilView;
//	ID3D11RenderTargetView *m_pRenderTargetView;
//
//	D3D_DRIVER_TYPE m_DriverType;
//	D3D_FEATURE_LEVEL m_FeatureLevel;
//	D3D11_TEXTURE2D_DESC backBufferDesc;
//	HWND                                            m_Handle;
//};
//

class Mesh;

// Platform independent renderer base class
// Basically represents a graphics context and its active shaders
class Renderer
{
	// Data
public:

	// Structors
public:
	Renderer();
	virtual ~Renderer();

	// Gets/sets
public:

	// Functions
public:
	virtual bool InitDevice(const int &width, const int &height) = 0;
	virtual bool CreateRenderTarget(const int &width, const int &height) = 0;
	virtual void SwapBuffer() = 0;
	virtual void ClearScreen() = 0;
	virtual void ShutDown() = 0;
};

