///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <tgaimage.h>
//Purpose: <Rerad the texture from tga files>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017,debug x86 , win10>
//Project: <jihoon.bae_jihoon.bae_CS350_3>
//Author: <jihoon.bae>
//Creation date: <23/10/2018>
//End Header --------------------------------------------------------*/
#ifndef TGAIMAGE_H
#define TGAIMAGE_H

#include "Main/Support.h"

// ---------------------------------------------------------------------------
// typedef for basic variable types

typedef char				s8;
typedef unsigned char		u8;
typedef signed short		s16;
typedef unsigned short		u16;
typedef signed long			s32;
typedef unsigned long		u32;
typedef signed long long	s64;
typedef unsigned long long	u64;
typedef float				f32;
typedef double				f64;
/////////////////////////////////////////////////////////////////////////////

class Image
{
public:
        static Image*	Load	(const char* pName);
        static void		Free	(Image* pImage);

        const u8* Data	() const	{	return mpData;	}
        u8*				Data	()			  {	return mpData;	}
        u32				BPP		() const	{	return mBPP;	}
        u32				SizeX	() const	{	return mSizeX;	}
        u32				SizeY	() const	{	return mSizeY;	}

private:
        u8*				mpData;
        u32				mBPP;
        u32				mSizeX;
        u32				mSizeY;

        // ---------------------------------------------------------------------------

                                        Image	();
                                        Image	(const Image& rhs);
                                        ~Image	();

        // ---------------------------------------------------------------------------

        bool			loadTGA	(u8* pData, u32 dataSize);
};

#endif // TGAIMAGE_H
