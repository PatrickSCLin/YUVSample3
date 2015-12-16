#include "pch.h"
#include "YUVDrawSession.h"

using namespace Win2D_YUV;
using namespace Platform;
using namespace Windows::Foundation;


YUVDrawSession::YUVDrawSession()
{
	HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory));

	ComPtr<IWICFormatConverter> wic_converter;

	result = wic_factory->CreateFormatConverter(&wic_converter);

	result = wic_converter.As(&wic_planar_converter);
}

 YUVDrawSession::~YUVDrawSession()
{
	wic_planar_converter.Get()->Release();

	wic_factory.Get()->Release();
}

void YUVDrawSession::DrawImage(CanvasDrawingSession^ session, const Platform::Array<byte>^ dataY, const Platform::Array<byte>^ dataU, const Platform::Array<byte>^ dataV, int width, int height)
{
	IWICBitmapSource* wic_bitmapSources[3];

	WICPixelFormatGUID formats[] = { GUID_WICPixelFormat8bppY, GUID_WICPixelFormat8bppCb, GUID_WICPixelFormat8bppCr };

	Size sizes[3] = { Size((float)width, (float)height), Size((float)width / 2, (float)height / 2), Size((float)width / 2, (float)height / 2) };

	UINT lengths[3] = { dataY->Length, dataU->Length, dataV->Length };

	byte* datas[3] = { dataY->Data, dataU->Data, dataV->Data };

	for (int i = 0; i < 3; i++)
	{
		HRESULT result = wic_factory->CreateBitmapFromMemory((int)sizes[i].Width, (int)sizes[i].Height, formats[i], (int)sizes[i].Width, lengths[i], datas[i], &wic_bitmaps[i]);

		result = result = wic_bitmaps[i].Get()->QueryInterface<IWICBitmapSource>(&wic_bitmapSources[i]);

		if (result == S_OK)
		{
			int test = 1;
		}
	}

	HRESULT result = wic_planar_converter->Initialize(wic_bitmapSources, 3, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherType::WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteType::WICBitmapPaletteTypeMedianCut);

	ComPtr<ID2D1DeviceContext> d2d_context = GetWrappedResource<ID2D1DeviceContext>(session);

	ComPtr<ID2D1Bitmap> d2d_bitmap;

	result = d2d_context->CreateBitmapFromWicBitmap(wic_planar_converter.Get(), &d2d_bitmap);

	session->Antialiasing = CanvasAntialiasing::Aliased;

	session->Clear(Windows::UI::Colors::Black);

	session->DrawImage(GetOrCreate<CanvasBitmap>(session->Device, d2d_bitmap.Get()), Rect(0, 0, d2d_context->GetSize().width, d2d_context->GetSize().height));

	for (int i = 0; i < 3; i++)
	{
		IWICBitmap* wic_bitmap = wic_bitmaps[i].Get();

		wic_bitmap->Release();

		wic_bitmap = NULL;
	}
}
