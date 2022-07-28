

#include <iostream>
#include <vector>



#include "LibVideoIOYuv.h"

using namespace std;

// ====================================================================================================================
// Local Functions
// ====================================================================================================================


/******************************************************************************
Description: 
Read width*height pixels from fd into dst. Input may be either 
8bit or 16bit little-endian lsb-aligned words.
Input para:
@param dst          destination image plane
@param fd           input file stream
@param is16bit      true if input file carries > 8bit data, false otherwise.
@param stride444    distance between vertically adjacent pixels of dst.
@param width444     width of active area in dst.
@param height444    height of active area in dst.
@param compID       chroma component
@param destFormat   chroma format of image
@param fileFormat   chroma format of file
@param fileBitDepth component bit depth in file

Return value:
@return true for success, false in case of error
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
static Bool read_plane(Pel* dst,
					   istream& fd,
	                   Bool is16bit,
	                   UInt stride444,
	                   UInt width444,
	                   UInt height444,
	                   const ComponentID compID,
	                   const ChromaFormat destFormat,
	                   const ChromaFormat fileFormat,
	                   const UInt fileBitDepth)
{
	const UInt csxFile = get_component_scaleX(compID, fileFormat);// csx: chroma subsampling x
	const UInt csyFile = get_component_scaleY(compID, fileFormat);
	const UInt csxDest = get_component_scaleX(compID, destFormat);
	const UInt csyDest = get_component_scaleY(compID, destFormat);

	const UInt widthDest = width444 >> csxDest;
	const UInt heightDest = height444 >> csyDest;
	const UInt strideDest = stride444 >> csxDest;

	const UInt strideFile = (width444 * (is16bit ? 2 : 1)) >> csxFile;
	std::vector<UChar> bufVec(strideFile);
	UChar* buf = &(bufVec[0]);

	if (compID != COMPONENT_Y && (fileFormat == CHROMA_400 || destFormat == CHROMA_400))
	{
		if (destFormat != CHROMA_400)
		{
			// Because when 400 converts to 420, the Cb and Cr chroma component is missing, 
			// so set chrominance data to mid-range: (1 << (fileBitDepth - 1)).
			const Pel value = Pel(1 << (fileBitDepth - 1));
			for (UInt y = 0; y < heightDest; y++, dst += strideDest)
			{
				for (UInt x = 0; x < widthDest; x++)
				{
					dst[x] = value;
				}
			}
		}

		if (fileFormat != CHROMA_400)
		{
			const UInt heightFile = height444 >> csyFile;
			fd.seekg(heightFile*strideFile, ios::cur);
			if (fd.eof() || fd.fail())
			{
				return false;
			}
		}
	} 
	else
	{
		const UInt maskYFile = (1 << csyFile) - 1;
		const UInt maskYDest = (1 << csyDest) - 1;
		for (UInt y444 = 0; y444 < height444; y444++)
		{
			// Save even position Pel.
			if ((y444 & maskYFile) == 0)
			{
				// Read a new line.
				fd.read(reinterpret_cast<TChar*>(buf), strideFile);
				if (fd.eof() || fd.fail())
				{
					return false;
				}
			}

			if ((y444 & maskYDest) == 0)
			{
				// Process current destination line.
				if (csxFile < csxDest)
				{
					// eg. file is 444, dest is 422.
					const UInt sx = csxDest - csxFile;
					if (!is16bit)
					{
						for (UInt x = 0; x < widthDest; x++)
						{
							dst[x] = buf[x << sx];
						}
					} 
					else
					{
						// High bit.
						for (UInt x = 0; x < widthDest; x++)
						{
							dst[x] = Pel(buf[(x << sx) * 2 + 0]) | (Pel(buf[(x << sx) * 2 + 1]) << 8);
						}
					}

				} 
				else
				{
					// eg. file is 422, dest is 444.
					const UInt sx = csxFile - csxDest;
					if (!is16bit)
					{
						for (UInt x = 0; x < widthDest; x++)
						{
							dst[x] = buf[x >> sx];
						}
					} 
					else
					{
						for (UInt x = 0; x < widthDest; x++)
						{
							dst[x] = Pel(buf[(x >> sx) * 2 + 0]) | (Pel(buf[(x >> sx) * 2 + 1]) << 8);
						}
					}

				}

				dst += strideDest;
			}			
		}
	}
	return true;
}


/*************************************************************************************
Description:
Scale all pixels in img depending upon sign of shiftbits by a factor of 2 shiftbits.
eg. 8bit¡ª¡ª>10bit, or 10bit¡ª¡ª>8bit etc.
Input para:
@param img        pointer to image to be transformed
@param stride     distance between vertically adjacent pixels of img
@param width      width of active area in img
@param height     height of active area in img
@param shiftbits  if zero, no operation performed
                  if > 0, multiply by 2^shiftbits
                  if < 0, divide and round by 2^shiftbits and clip
@param minval     minimum clipping value when dividing
@param maxval     maximum clipping value when dividing
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
************************************************************************************/
static Void scale_plane(Pel* img, const UInt stride, const UInt width, const UInt height, Int shiftBits, Pel minVal, Pel maxVal)
{
	if (shiftBits > 0)
	{
		for (UInt y = 0; y < height; y++, img += stride)
		{
			for (UInt x = 0; x < width; x++)
			{
				img[x] <<= shiftBits;
			}
		}
	} 
	else if(shiftBits < 0)
	{

		shiftBits = -shiftBits;

		Pel rounding = 1 << (shiftBits - 1);
		for (UInt y = 0; y < height; y++, img += stride)
		{
			for (UInt x = 0; x < width; x++)
			{
				img[x] = Clip3(minVal, maxVal, Pel((img[x] + rounding) >> shiftBits));
			}
		}
	}


}




// ====================================================================================================================
// Public member functions
// ====================================================================================================================




/******************************************************************************
Description: 
Open file for reading/writing Y'CbCr frames.
Frames read/written have bitdepth fileBitDepth, and are automatically
formatted as 8 or 16 bit word values.
Input para:
\param fileName          file name string
\param writeMode         file open mode: true=write, false=read
\param fileBitDepth      bit-depth array of input/output file data.
\param MSBExtendedBitDepth
\param internalBitDepth  bit-depth array to scale image data to/from when reading/writing.
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Void LibVideoIOYuv::Open(const std::string& fileName, Bool writeMode, const Int fileBitDepth[MAX_NUM_CHANNEL_TYPE], const Int internalBitDepth[MAX_NUM_CHANNEL_TYPE])
{
	//NOTE: files cannot have bit depth greater than 16.
	for (UInt ch = 0; ch < MAX_NUM_CHANNEL_TYPE; ch++)
	{
		m_iFileBitDepth[ch] = std::min<UInt>(fileBitDepth[ch], 16);
		m_iBitDepthShift[ch] = internalBitDepth[ch] - fileBitDepth[ch];

		if (m_iFileBitDepth[ch] > 16)
		{
			if (writeMode)
			{
				std::cerr << "\nWARNING: Cannot write a yuv file of bit depth greater than 16 - output will be right-shifted down to 16-bit precision\n" << std::endl;
			}
			else
			{
				std::cerr << "\nERROR: Cannot read a yuv file of bit depth greater than 16\n" << std::endl;
				exit(0);
			}
		}
	}

	if (writeMode)
	{
		m_cHandle.open(fileName.c_str(), ios::binary | ios::out);

		if (m_cHandle.fail())
		{
			printf("\nfailed to write reconstructed YUV file\n");
			exit(0);
		}
	}
	else
	{
		m_cHandle.open(fileName.c_str(), ios::binary | ios::in);

		if (m_cHandle.fail())
		{
			printf("\nfailed to open Input YUV file\n");
			exit(0);
		}
	}

	return;

}







Bool LibVideoIOYuv::Is_eof()
{
	return m_cHandle.eof();
}


/******************************************************************************
Description: Read one Y'CbCr frame
Input para: 
Return value:
Author£ºDe-Kai Chen    <cdk5@foxmail.com>
******************************************************************************/
Bool LibVideoIOYuv::Read(LibComPicYuv* picYuvOrg, ChromaFormat format)
{
	// Check end-of-file.
	if (Is_eof())
	{
		return false;
	}

	if (format >= NUM_CHROMA_FORMAT)
	{
		format = picYuvOrg->Get_chroma_format();
	}

	Bool is16bit = false;

	for (UInt ch = 0; ch < MAX_NUM_CHANNEL_TYPE; ch++)
	{
		if (m_iFileBitDepth[ch] > 8)
		{
			is16bit = true;
		}
	}


	const UInt stride444 = picYuvOrg->Get_stride(COMPONENT_Y);

	// Compute actual YUV width & height.
	const UInt width444 = picYuvOrg->Get_width(COMPONENT_Y);
	const UInt height444 = picYuvOrg->Get_height(COMPONENT_Y);

	
	for (UInt comp = 0; comp < MAX_NUM_COMPONENT; comp++)
	{

		const ComponentID compID = ComponentID(comp);
		const ChannelType chType = to_channel_type(compID);

		const Pel minVal = 0;
		const Pel maxVal = (1 << (m_iFileBitDepth[chType] + m_iBitDepthShift[chType])) - 1;


		if (!read_plane(picYuvOrg->Get_addr(compID), m_cHandle, is16bit, stride444, width444, height444, compID, picYuvOrg->Get_chroma_format(), format, m_iFileBitDepth[chType]))
		{
			return false;
		}


		if (compID < picYuvOrg->Get_number_valid_components())
		{
			const UInt csx = get_component_scaleX(compID, picYuvOrg->Get_chroma_format());
			const UInt csy = get_component_scaleY(compID, picYuvOrg->Get_chroma_format());
			scale_plane(picYuvOrg->Get_addr(compID), stride444 >> csx, width444 >>csx, height444 >> csy, m_iBitDepthShift[chType], minVal, maxVal);
		}

	}

	return true;
}


