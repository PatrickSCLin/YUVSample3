#include "pch.h"
#include "YUVDrawSession.h"

using namespace Win2D_YUV;
using namespace Platform;
using namespace Windows::Foundation;

static int maxTextureSize = 1920;

YUVDrawSession::YUVDrawSession()
{
	HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory));

	ComPtr<IWICFormatConverter> wic_converter;

	result = wic_factory->CreateFormatConverter(&wic_converter);

	result = wic_converter.As(&wic_planar_converter);

	result = wic_factory->CreateBitmapScaler(&wic_scaler);
}

 YUVDrawSession::~YUVDrawSession()
{
	wic_planar_converter.Get()->Release();

	wic_factory.Get()->Release();
}

void YUVDrawSession::DrawImage(CanvasDrawingSession^ session, int32 dataPtrY, int32 dataPtrU, int32 dataPtrV, int width, int height)
{
	IWICBitmapSource* wic_bitmapSources[3];

	WICPixelFormatGUID formats[] = { GUID_WICPixelFormat8bppY, GUID_WICPixelFormat8bppCb, GUID_WICPixelFormat8bppCr };

	Size sizes[3] = { Size((float)width, (float)height), Size((float)width / 2, (float)height / 2), Size((float)width / 2, (float)height / 2) };

	UINT lengths[3] = { width * height, (width / 2) * (height / 2) , (width / 2) * (height / 2) };
	
	byte* datas[3] = { (byte*)(void*)IntPtr(dataPtrY), (byte*)(void*)IntPtr(dataPtrU), (byte*)(void*)IntPtr(dataPtrV) };
	
	for (int i = 0; i < 3; i++)
	{
		HRESULT result = wic_factory->CreateBitmapFromMemory((int)sizes[i].Width, (int)sizes[i].Height, formats[i], (int)sizes[i].Width, lengths[i], datas[i], &wic_bitmaps[i]);

		result = result = wic_bitmaps[i].Get()->QueryInterface<IWICBitmapSource>(&wic_bitmapSources[i]);
	}

	HRESULT result = wic_planar_converter->Initialize(wic_bitmapSources, 3, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherType::WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteType::WICBitmapPaletteTypeCustom);

	float scale = (maxTextureSize > 0) ? (float)maxTextureSize / (float)max(width, height) : 1.0f;

	int scaledWidth = (int)(scale * width);

	int scaledHeight = (int)(scale * height);

	result = wic_scaler->Initialize(wic_planar_converter.Get(), scaledWidth, scaledHeight, WICBitmapInterpolationMode::WICBitmapInterpolationModeNearestNeighbor);

	ComPtr<ID2D1DeviceContext> d2d_context = GetWrappedResource<ID2D1DeviceContext>(session);

	ComPtr<ID2D1Bitmap> d2d_bitmap;

	result = d2d_context->CreateBitmapFromWicBitmap(wic_scaler.Get(), &d2d_bitmap);

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
