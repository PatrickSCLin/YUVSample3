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

		void YUVDrawSession::DrawImage(CanvasDrawingSession^ session, int32 dataPtrY, int32 dataPtrU, int32 dataPtrV, int width, int height);

	private:

		ComPtr<IWICImagingFactory> wic_factory;

		ComPtr<IWICPlanarFormatConverter> wic_planar_converter;

		ComPtr<IWICBitmapScaler> wic_scaler;

		ComPtr<IWICBitmap> wic_bitmaps[3];

		YUVDrawSession::YUVDrawSession();

		YUVDrawSession::~YUVDrawSession();

    };
}
