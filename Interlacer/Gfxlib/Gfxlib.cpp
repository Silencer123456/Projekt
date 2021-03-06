#include "stdafx.h"

#pragma warning(disable:4251)  //ignorovani varovani o exportu STL trid - tyto tridy mimo toto DLL nebudou pouzity

#include "Gfxlib.h"
#include "GfxlibErrors.h"
#include <iostream>
#include <fstream>
#include <string>
#include <Magick++.h>

void log(std::string str)
{
	std::ofstream file("log.txt");
	file << str;
	file.close();
}

void setResourceLimits()
{
#if defined _WIN64
	Magick::ResourceLimits::memory(68719476736);
	Magick::ResourceLimits::disk(0);
#else
	Magick::ResourceLimits::memory(4294967296);
	Magick::ResourceLimits::disk(0);
#endif
}

void* createImage(int w, int h)
{
	std::string wStr = std::to_string(w);
	std::string hStr = std::to_string(h);
	std::string sizeStr = wStr + std::string("x") + hStr;
	Magick::Image* img = nullptr;
	try
	{
		img = new Magick::Image;
		img->size(sizeStr);
	}
	catch (...)
	{
		delete img;
		RaiseException(GfxlibErrors::PictureCreationFailure, 0, 0, 0);
	}
	return (void*)img;
}

void deleteImage(void* ptr)
{
	Magick::Image* imagePtr = (Magick::Image*)ptr;
	imagePtr->syncPixels();
	delete imagePtr;
}

INT64* getPixelDataPtr(void* ptr)
{
	Magick::Image* imagePtr = (Magick::Image*)ptr;
	INT64* pxPtr = nullptr;
	try
	{
		pxPtr = (INT64*)imagePtr->getPixels(0, 0, imagePtr->columns(), imagePtr->rows());
	}
	catch (...)
	{
		RaiseException(GfxlibErrors::PictureCreationFailure, 0, 0, 0);
	}
	return pxPtr;
}

void* loadImage(char* filename)
{
	Magick::Image* img = nullptr;
	try
	{
		img = new Magick::Image;
		img->read(filename);
	}
	catch (...)
	{
		delete img;
		RaiseException(GfxlibErrors::PictureLoadFailure, 0, 0, 0);
	}
	return (void*)img;
}

void getImageSizeWithoutLoading(char* filename, int* width, int* height)
{
	try
	{
		Magick::Image img;
		img.ping(filename);
		*width = (int)img.columns();
		*height = (int)img.rows();
	}
	catch (...)
	{
		RaiseException(GfxlibErrors::PictureLoadFailure, 0, 0, 0);
	}
}

void saveImage(void* ptr, char* filename)
{
	try
	{
		Magick::Image* imagePtr = (Magick::Image*)ptr;
		imagePtr->syncPixels();
		imagePtr->write(filename);
	}
	catch (...)
	{
		RaiseException(GfxlibErrors::PictureSaveFailure, 0, 0, 0);
	}
}

int getImageWidth(void* ptr)
{
	Magick::Image* imagePtr = (Magick::Image*)ptr;
	return (int)imagePtr->columns();
}

int getImageHeight(void* ptr)
{
	Magick::Image* imagePtr = (Magick::Image*)ptr;
	return (int)imagePtr->rows();
}

void resizeImage(void* ptr, int w, int h, int filterType)
{
	try
	{
		std::string wStr = std::to_string(w);
		std::string hStr = std::to_string(h);
		std::string sizeStr = wStr + std::string("x") + hStr + std::string("!");
		Magick::Image* imagePtr = (Magick::Image*)ptr;
		imagePtr->syncPixels();
		imagePtr->filterType((MagickCore::FilterTypes)filterType);
		imagePtr->resize(sizeStr);
	}
	catch (...)
	{
		RaiseException(GfxlibErrors::PictureResizeFilure, 0, 0, 0);
	}
}