#include "image.h"
#include "helpers.h"
#include <wincodec.h>

#pragma comment (lib, "windowscodecs.lib")

namespace TowerDefense
{
	Image::Image() :
		m_pixels(nullptr),
		m_width(0),
		m_height(0) {}
	Image::Image(unsigned width, unsigned height) :
		m_pixels(new Color[width * height]),
		m_width(width),
		m_height(height) {}
	Image::Image(const Image& image) :
		m_pixels(new Color[(size_t)image.m_width * image.m_height]),
		m_width(image.m_width),
		m_height(image.m_height)
	{
		for (unsigned y = 0; y < m_height; y++)
			for (unsigned x = 0; x < m_width; x++)
				(*this)(x, y) = image(x, y);
	}
	Image::Image(Image&& other) noexcept :
		m_pixels(other.m_pixels),
		m_width(other.m_width),
		m_height(other.m_height)
	{
		other.m_pixels = nullptr;
		other.m_width = 0;
		other.m_height = 0;
	}
	Image::Image(const wchar_t* filename) : Image()
	{
		LoadFromFile(filename);
	}
	Image::~Image()
	{
		Delete();
	}

	void Image::LoadFromFile(const wchar_t* filename)
	{
		Delete();
		Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
		Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
		Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
		Microsoft::WRL::ComPtr<IWICFormatConverter> converter;

		ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, static_cast<LPVOID*>(&factory)));
		ThrowIfFailed(factory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder));
		ThrowIfFailed(decoder->GetFrame(0, &frame));
		ThrowIfFailed(factory->CreateFormatConverter(&converter));
		ThrowIfFailed(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom));
		ThrowIfFailed(converter->GetSize(&m_width, &m_height));
		m_pixels = new Color[m_width * m_height];
		ThrowIfFailed(m_pixels ? S_OK : E_OUTOFMEMORY);
		ThrowIfFailed(converter->CopyPixels(nullptr, m_width * 4, m_width * m_height * sizeof(Color), reinterpret_cast<BYTE*>(m_pixels)));
	}

	void Image::SaveToPNG(const wchar_t* filename) const
	{
		if (m_pixels)
		{
			Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
			Microsoft::WRL::ComPtr<IWICStream> stream;
			Microsoft::WRL::ComPtr<IWICBitmapEncoder> encoder;
			Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> bitmapFrame;
			Microsoft::WRL::ComPtr<IPropertyBag2> propertybag;

			ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, static_cast<LPVOID*>(&factory)));
			ThrowIfFailed(factory->CreateStream(&stream));
			ThrowIfFailed(stream->InitializeFromFilename(filename, GENERIC_WRITE));
			ThrowIfFailed(factory->CreateEncoder(GUID_ContainerFormatPng, NULL, &encoder));
			ThrowIfFailed(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache));
			ThrowIfFailed(encoder->CreateNewFrame(&bitmapFrame, &propertybag));
			ThrowIfFailed(bitmapFrame->Initialize(propertybag.Get()));
			ThrowIfFailed(bitmapFrame->SetSize(m_width, m_height));
			WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
			ThrowIfFailed(bitmapFrame->SetPixelFormat(&formatGUID));
			ThrowIfFailed(IsEqualGUID(formatGUID, GUID_WICPixelFormat32bppBGRA) ? S_OK : E_FAIL);
			UINT stride = m_width * 4;
			UINT imageSize = m_height * stride;
			ThrowIfFailed(bitmapFrame->WritePixels(m_height, stride, imageSize, reinterpret_cast<BYTE*>(m_pixels)));
			ThrowIfFailed(bitmapFrame->Commit());
			ThrowIfFailed(encoder->Commit());
		}
	}

	Image& Image::operator=(const Image& image)
	{
		Delete();
		m_width = image.m_width;
		m_height = image.m_height;
		m_pixels = new Color[m_width * m_height];
		for (unsigned y = 0; y < m_height; y++)
			for (unsigned x = 0; x < m_width; x++)
				(*this)(x, y) = image(x, y);
		return *this;
	}
	Image& Image::operator=(Image&& image) noexcept
	{
		m_pixels = image.m_pixels;
		m_width = image.m_width;
		m_height = image.m_height;
		image.m_pixels = nullptr;
		image.m_width = 0;
		image.m_height = 0;
		return *this;
	}
	void Image::Swap(Image& image) noexcept
	{
		Color* pixels = image.m_pixels;
		unsigned width = image.m_width;
		unsigned height = image.m_height;
		image.m_pixels = m_pixels;
		image.m_width = m_width;
		image.m_height = m_height;
		m_pixels = pixels;
		m_width = width;
		m_height = height;
	}
	void Image::Recolor(Color c)
	{
		for (unsigned y = 0; y < m_height; y++)
			for (unsigned x = 0; x < m_width; x++)
				(*this)(x, y) = c;
	}
	void Image::Delete()
	{
		if (m_pixels)
		{
			delete[] m_pixels;
			m_width = 0;
			m_height = 0;
		}
	}
}