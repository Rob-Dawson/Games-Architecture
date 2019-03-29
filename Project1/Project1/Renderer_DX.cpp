#include "Renderer_DX.h"
#include <d3dcompiler.h>
#include <exception>

struct ShapeBuffer
{
	DirectX::XMFLOAT4X4 MVM;
	DirectX::XMFLOAT4	Colour;
};

/******************************************************************************************************************/

Renderer_DX::Renderer_DX(HWND hWnd)
	: m_Handle(hWnd), m_Device{ nullptr }, m_SwapChain{ nullptr },
	m_DeviceContext{ nullptr }, m_RenderTargetView{ nullptr },
	m_DriverType{ D3D_DRIVER_TYPE_NULL }, m_FeatureLevel{ D3D_FEATURE_LEVEL_11_0 }
{
}

/******************************************************************************************************************/

bool Renderer_DX::InitDevice(const int &width, const int &height)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc{ 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_Handle;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	D3D_DRIVER_TYPE driverType[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	D3D_FEATURE_LEVEL featureLevel[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	auto const numFeatureLevels = ARRAYSIZE(featureLevel);

	auto result = 0;
	for (const auto driver : driverType)
	{
		result = D3D11CreateDeviceAndSwapChain(
			nullptr, driver, nullptr, 0,
			featureLevel, numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc,
			&m_SwapChain, &m_Device, &m_FeatureLevel, &m_DeviceContext);
		if (SUCCEEDED(result))
		{
			m_DriverType = driver;
		}
	}

	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create device and or swap chain", L"Error in Renderer_DX", MB_OK);
		return result;
	}

	if (CreateRenderTarget(width, height));
	if (InitialiseShaders());
	return true;
}

/******************************************************************************************************************/

bool Renderer_DX::CreateRenderTarget(const int &width, const int &height)
{
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT result = m_Device->CreateTexture2D(&descDepth, nullptr, &m_DepthStencil);

	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create depth stencil texture", L"Error in Renderer", MB_OK);
		return result;
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	result = m_Device->CreateDepthStencilView(m_DepthStencil, &descDSV, &m_DepthStencilView);
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create depth stencil view", L"Error in Renderer", MB_OK);
		return result;
	}

	ID3D11Texture2D *m_BackBuffer{ nullptr };
	result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_BackBuffer));
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create Back buffer", L"Error in Renderer", MB_OK);
		return result;
	}

	result = m_Device->CreateRenderTargetView(m_BackBuffer, nullptr, &m_RenderTargetView);
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create render target", L"Error in Renderer", MB_OK);
		return result;
	}
	m_BackBuffer->GetDesc(&m_BackBufferDesc);
	m_BackBuffer->Release();

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_DeviceContext->RSSetViewports(1, &vp);

	return true;
}

/******************************************************************************************************************/

bool Renderer_DX::CompileShaderFromFile(const WCHAR* const szFileName, const LPCSTR const szEntryPoint, const LPCSTR const szShaderModel, ID3DBlob** const ppBlobOut) const
{
	auto hr = 0;

	const DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return 0;
}

/******************************************************************************************************************/

bool Renderer_DX::InitialiseShaders()
{
	//Create Vertex Shader
	ID3DBlob *vsBlob = nullptr;
	CompileShaderFromFile(L"Cube.fx", "VS_main", "vs_4_0", &vsBlob);
	HRESULT hr = m_Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_VertexShader);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Error creating vertex shader", L"Error in Renderer_DX", MB_OK);
		return hr;
	}

	//Create Pixel Shader
	ID3DBlob *psBlob = nullptr;
	CompileShaderFromFile(L"Cube.fx", "PS_main", "ps_4_0", &psBlob);
	hr = m_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PixelShader);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Error creating Pixel Shader", L"Error in Renderer_DX", MB_OK);
		return hr;
	}

	//Create Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	auto layoutSize = ARRAYSIZE(layout);

	hr = m_Device->CreateInputLayout(layout, layoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_InputLayout);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"Error creating Input Layout", L"Error in Renderer", MB_OK);
		return hr;
	}
	m_DeviceContext->VSSetShader(m_VertexShader, 0, 0);
	m_DeviceContext->PSSetShader(m_PixelShader, 0, 0);
	m_DeviceContext->IASetInputLayout(m_InputLayout);
	return true;
}

/******************************************************************************************************************/

void Renderer_DX::ClearScreen()
{
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, DirectX::Colors::MidnightBlue);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

/******************************************************************************************************************/

void Renderer_DX::SwapBuffer()
{
	m_SwapChain->Present(0, 0);
}

/******************************************************************************************************************/


void Renderer_DX::ShutDown()
{
	if (m_Device) m_Device->Release();
	if (m_SwapChain) m_SwapChain->Release();
	if (m_PixelShader) m_PixelShader->Release();
	if (m_ShapeBuffer) m_ShapeBuffer->Release();
	if (m_VertexShader) m_VertexShader->Release();
	if (m_DeviceContext) m_DeviceContext->Release();
	if (m_RenderTargetView) m_RenderTargetView->Release();

}

/******************************************************************************************************************/

Renderer_DX::~Renderer_DX() = default;