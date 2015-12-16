#pragma once

using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::WRL;

namespace Win2D_YUV
{
	public ref class YUVDrawSession sealed
    {
    public:

		static property YUVDrawSession^ SharedSession
		{
			YUVDrawSession^ get()
			{
				static YUVDrawSession^ instance = ref new YUVDrawSession();

				return instance;
			}
		}

		void DrawImage(CanvasDrawingSession^ session, const Platform::Array<byte>^ dataY, const Platform::Array<byte>^ dataU, const Platform::Array<byte>^ dataV, int width, int height);

	private:

		ComPtr<IWICImagingFactory> wic_factory;

		ComPtr<IWICBitmap> wic_bitmaps[3];

		ComPtr<IWICPlanarFormatConverter> wic_planar_converter;

		YUVDrawSession::YUVDrawSession();

		YUVDrawSession::~YUVDrawSession();

    };
}
