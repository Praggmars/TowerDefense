#include "image.h"
#include <wincodec.h>

#pragma comment (lib, "windowscodecs.lib")

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

bool Image::LoadFromFile(const wchar_t* filename)
{
	Delete();
	struct WIC_Stuff
	{
		IWICImagingFactory* factory;
		IWICBitmapDecoder* decoder;
		IWICBitmapFrameDecode* frame;
		IWICFormatConverter* converter;

		WIC_Stuff() :
			factory(nullptr),
			decoder(nullptr),
			frame(nullptr),
			converter(nullptr) {}
		~WIC_Stuff()
		{
			if (factory)
				factory->Release();
			if (decoder)
				decoder->Release();
			if (frame)
				frame->Release();
			if (converter)
				converter->Release();
		}
	} wic_stuff;

	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)(&wic_stuff.factory)))) return false;
	if (FAILED(wic_stuff.factory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wic_stuff.decoder))) return false;
	if (FAILED(wic_stuff.decoder->GetFrame(0, &wic_stuff.frame))) return false;
	if (FAILED(wic_stuff.factory->CreateFormatConverter(&wic_stuff.converter))) return false;
	if (FAILED(wic_stuff.converter->Initialize(wic_stuff.frame, GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom))) return false;
	if (FAILED(wic_stuff.converter->GetSize(&m_width, &m_height))) return false;
	m_pixels = new Color[m_width * m_height];
	if (FAILED(m_pixels ? S_OK : E_OUTOFMEMORY)) return false;
	if (FAILED(wic_stuff.converter->CopyPixels(nullptr, m_width * 4, m_width * m_height * sizeof(Color), reinterpret_cast<BYTE*>(m_pixels)))) return false;

	return true;
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