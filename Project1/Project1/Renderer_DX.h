#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include "Renderer.h"

class Renderer_DX : public Renderer
{
public:
	Renderer_DX(HWND hWnd);
	virtual ~Renderer_DX();

	//Data
protected:
	HWND					m_Handle;
	D3D_DRIVER_TYPE			m_DriverType;
	D3D_FEATURE_LEVEL		m_FeatureLevel;
	D3D11_TEXTURE2D_DESC	m_BackBufferDesc;

	ID3D11Device			*m_Device;
	IDXGISwapChain			*m_SwapChain;
	ID3D11InputLayout		*m_InputLayout;
	ID3D11PixelShader		*m_PixelShader;
	ID3D11Buffer			*m_ShapeBuffer;
	ID3D11Texture2D			*m_DepthStencil;
	ID3D11VertexShader		*m_VertexShader;
	ID3D11DeviceContext		*m_DeviceContext;
	ID3D11DepthStencilView  *m_DepthStencilView;
	ID3D11RenderTargetView	*m_RenderTargetView;

	//Gets/Sets
public:
	inline ID3D11Device *GetDevice() const
	{
		return m_Device;
	}

	inline ID3D11DeviceContext *GetDeviceContext() const
	{
		return m_DeviceContext;
	}

	void SetDevice(ID3D11Device *device)
	{
		m_Device = device;
	}

	void SetDeviceContext(ID3D11DeviceContext *deviceContext)
	{
		m_DeviceContext = deviceContext;
	}
	//Functions
	virtual bool InitDevice(const int &width, const int &height);
	virtual bool CreateRenderTarget(const int &width, const int &height);
	virtual void SwapBuffer();
	virtual void ClearScreen();
	
	
	bool CompileShaderFromFile(const WCHAR* const szFileName, const LPCSTR const szEntryPoint, const LPCSTR const szShaderModel,
		ID3DBlob** const ppBlobOut) const;
	
	
	bool InitialiseShaders();
	void Draw();
	void ShutDown();
};

