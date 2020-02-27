#pragma once

namespace TowerDefense
{
	struct Color
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;

		static Color White() { return Color(0xff, 0xff, 0xff, 0xff); }
		static Color Black() { return  Color(0x0, 0x00, 0x00, 0xff); }
		static Color Red() { return  Color(0xff, 0x00, 0x00, 0xff); }
		static Color Green() { return  Color(0x00, 0xff, 0x00, 0xff); }
		static Color Blue() { return  Color(0x00, 0x00, 0xff, 0xff); }
		static Color Yellow() { return  Color(0xff, 0xff, 0x00, 0xff); }
		static Color Magenta() { return  Color(0xff, 0x00, 0xff, 0xff); }
		static Color Cyan() { return  Color(0x00, 0xff, 0xff, 0xff); }
		static Color Transparent() { return Color(0x00, 0x00, 0x00, 0x00); }

		Color() : r(0x00), g(0x00), b(0x00), a(0xff) {}
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xff)
			: r(r), g(g), b(b), a(a) {}
		Color(unsigned c) :
			r(static_cast<unsigned char>((c >> 24) & 0xff)),
			g(static_cast<unsigned char>((c >> 16) & 0xff)),
			b(static_cast<unsigned char>((c >> 8) & 0xff)),
			a(static_cast<unsigned char>(c & 0xff)) {}

		inline operator unsigned() { return *reinterpret_cast<unsigned*>(this); }
		inline bool operator==(Color c) { return r == c.r && g == c.g && b == c.b && a == c.a; }
		inline bool operator!=(Color c) { return r != c.r || g != c.g || b != c.b || a != c.a; }

		inline Color& operator=(const Color& c) { r = c.r; g = c.g; b = c.b; a = c.a; return *this; }
		inline Color& operator=(unsigned c) { *this = Color(c); return *this; }
	};

	class Image
	{
		Color* m_pixels;
		unsigned m_width;
		unsigned m_height;

	public:
		Image();
		Image(unsigned width, unsigned height);
		Image(const Image& image);
		Image(Image&& image) noexcept;
		Image(const wchar_t* filename);
		~Image();

		void LoadFromFile(const wchar_t* filename);
		void SaveToPNG(const wchar_t* filename) const;
		Image& operator=(const Image& image);
		Image& operator=(Image&& image) noexcept;
		void Swap(Image& image) noexcept;

		void Recolor(Color c = Color::Black());
		void Delete();

		inline bool Empty() const { return m_pixels == nullptr; }
		inline operator bool() { return m_pixels != nullptr; }
		inline bool operator!() { return m_pixels == nullptr; }
		inline unsigned Width() const { return m_width; }
		inline unsigned Height() const { return m_height; }
		inline Color* Pixels() const { return m_pixels; }
		inline Color& operator()(unsigned x, unsigned y) { return m_pixels[m_width * y + x]; }
		inline Color operator()(unsigned x, unsigned y) const { return m_pixels[m_width * y + x]; }
	};
};