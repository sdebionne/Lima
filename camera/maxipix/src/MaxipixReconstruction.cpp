#include "MaxipixReconstruction.h"

using namespace lima::Maxipix;

static const int MAXIPIX_LINE_SIZE = 256 * 2;
static const int MAXIPIX_NB_LINE = 256;
static const int MAXIPIX_NB_COLUMN = 256;

//----------------------------------------------------------------------------
//			  5x1 copy function
//----------------------------------------------------------------------------
#define COPY_5x1				\
  int maxipix_line_size = MAXIPIX_LINE_SIZE;	\
  int depth = src.depth();			\
  if(depth == 4)				\
    maxipix_line_size <<= 1;				\
  /* set aSrcPt to the last line of the 5th chip */	\
  unsigned char *aSrcPt = ((unsigned char*)src.data()) +		\
			  (maxipix_line_size * 5 * MAXIPIX_NB_LINE) - maxipix_line_size; \
  /*set aDstPt to the last line of the 5th chip including the gap */	\
  unsigned char *aDstPt = ((unsigned char*)dst.data()) +		\
			  ((maxipix_line_size * 5 + xSpace * depth * 4) * MAXIPIX_NB_LINE) - maxipix_line_size;	\
									\
  int aNbLine = MAXIPIX_NB_LINE;					\
  do									\
    {									\
      int MAXIPIX_LINE_SIZE_COPY = maxipix_line_size;			\
									\
      int aNbChipCopyWithGap = 4;					\
      do								\
	{								\
	  memmove(aDstPt,aSrcPt,MAXIPIX_LINE_SIZE_COPY);		\
	  GAP_COPY_FUNCTION						\
	    aDstPt -= maxipix_line_size + xSpace * depth;		\
	  aSrcPt -= maxipix_line_size;					\
	}								\
      while(--aNbChipCopyWithGap);					\
      memmove(aDstPt,aSrcPt,MAXIPIX_LINE_SIZE_COPY);			\
      aDstPt -= maxipix_line_size;					\
      aSrcPt -= maxipix_line_size;					\
    }									\
while(--aNbLine);

static void _raw_5x1(Data &src,Data &dst,
		     int xSpace)
{
  if(src.depth() == 4)		// Accumulation
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION			\
      int *aPixel = ((int*)aDstPt) - 1;		\
      for(int i = xSpace;i;--i,--aPixel)	\
	*aPixel = 0;
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION					\
      unsigned short *aPixel = ((unsigned short*)aDstPt) - 1;	\
      for(int i = xSpace;i;--i,--aPixel)			\
	*aPixel = 0;
      COPY_5x1;
    }
}

static inline void _zero_5x1(Data &src,Data &dst,
			     int xSpace)
{
  if(src.depth() == 4)
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION				\
      MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 4;	\
      int*aPixel = ((int*)aDstPt);			\
      for(int i = xSpace + 2;i;--i,--aPixel)		\
	*aPixel = 0;
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION					\
      MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 2;		\
      unsigned short *aPixel = ((unsigned short*)aDstPt);	\
      for(int i = xSpace + 2;i;--i,--aPixel)			\
	*aPixel = 0;
      COPY_5x1;
    }
}
static inline void _dispatch_5x1(Data &src,Data &dst,
				 int xSpace)
{
  int nbPixelDispatch = (xSpace >> 1) + 1; // (xSpace / 2) + 1
  if(src.depth() == 4)
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 4;			\
      int *aPixel = ((int*)aDstPt);					\
      int aPixelValue = *aPixel / nbPixelDispatch;			\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue;						\
									\
      int *aSrcPixel = ((int*)aSrcPt) - 1;	\
      aPixelValue = *aSrcPixel / nbPixelDispatch;			\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue; 
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 2;			\
      unsigned short *aPixel = ((unsigned short*)aDstPt);		\
      unsigned short aPixelValue = *aPixel / nbPixelDispatch;		\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue;						\
									\
      unsigned short *aSrcPixel = ((unsigned short*)aSrcPt) - 1;	\
      aPixelValue = *aSrcPixel / nbPixelDispatch;			\
      for(int i = nbPixelDispatch;i;--i,--aPixel)			\
	*aPixel = aPixelValue; 
      COPY_5x1;
    }
}

static inline void _mean_5x1(Data &src,Data &dst,
			     int xSpace)
{
  int nbPixelMean = (xSpace >> 1) + 1; // (xSpace / 2) + 1
  if(src.depth() == 4)
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 4;			\
      int*aPixel = ((int*)aDstPt);					\
      int aFirstPixelValue = *aPixel / nbPixelMean;			\
      *aPixel = aFirstPixelValue;--aPixel;				\
									\
      int *aSrcPixel = ((int*)aSrcPt) - 1;				\
      int aSecondPixelValue = *aSrcPixel / nbPixelMean;			\
      float aStepValue = (aSecondPixelValue - aFirstPixelValue) / (xSpace + 1); \
      float aPixelValue = aFirstPixelValue + aStepValue;		\
      for(int i = xSpace + 1;i;--i,aPixelValue += aStepValue,--aPixel)	\
	*aPixel = (int)aPixelValue;
      COPY_5x1;
    }
  else
    {
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION						\
      MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 2;			\
      unsigned short *aPixel = ((unsigned short*)aDstPt);		\
      unsigned short aFirstPixelValue = *aPixel / nbPixelMean;		\
      *aPixel = aFirstPixelValue;--aPixel;				\
									\
      unsigned short *aSrcPixel = ((unsigned short*)aSrcPt) - 1;	\
      unsigned short aSecondPixelValue = *aSrcPixel / nbPixelMean;	\
      float aStepValue = (aSecondPixelValue - aFirstPixelValue) / (xSpace + 1); \
      float aPixelValue = aFirstPixelValue + aStepValue;		\
      for(int i = xSpace + 1;i;--i,aPixelValue += aStepValue,--aPixel)	\
	*aPixel = (unsigned short)aPixelValue;
      COPY_5x1;
    }
}
//----------------------------------------------------------------------------
//			     2x2 function
//----------------------------------------------------------------------------
template<class type>
static inline void copy_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  int aTotalHeight = (MAXIPIX_NB_LINE * 2) + ySpace;
  type *aSrcPt = (type*)src.data();
  type *aDstPt = ((type*)dst->data) + (aTotalWidth * aTotalHeight) - aTotalWidth;

  int aJump2LeftChip = aTotalWidth - 1;
  
  int aColumnIterNumber = MAXIPIX_NB_COLUMN;
  do
    {
      //copy left chips
      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt -= aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;
 
      aDstPt -= aTotalWidth * ySpace;
      
      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt -= aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;
 
      aDstPt += aJump2LeftChip + aTotalWidth;
      --aJump2LeftChip;
      //copy right chips
      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;

      aDstPt += aTotalWidth * ySpace;

      for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth,++aSrcPt)
	*aDstPt = *aSrcPt;

      aDstPt -= aJump2LeftChip + aTotalWidth;
      --aJump2LeftChip;
    }
  while(--aColumnIterNumber);

}

template<class type>
static inline void _raw_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  copy_2x2<type>(src,dst,xSpace,ySpace);

  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN;
  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace;++k)
      aDstPt[k] = 0;
  
  aDstPt -= MAXIPIX_NB_COLUMN;
  int aGapSize = aTotalWidth * ySpace;
  memset(aDstPt,0,aGapSize << 1);
  aDstPt += aGapSize;

  aDstPt += MAXIPIX_NB_COLUMN;
  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace;++k)
      aDstPt[k] = 0;
}

template<class type>
static inline void _zero_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  copy_2x2<type>(src,dst,xSpace,ySpace);
  
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  for(int i = MAXIPIX_NB_LINE - 1;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace + 2;++k)
      aDstPt[k] = 0;
  
  aDstPt -= MAXIPIX_NB_COLUMN - 1;
  int aGapSize = aTotalWidth * (ySpace + 2);
  memset(aDstPt,0,aGapSize << 1);
  aDstPt += aGapSize;

  aDstPt += MAXIPIX_NB_COLUMN - 1;
  for(int i = MAXIPIX_NB_LINE - 1;i;--i,aDstPt += aTotalWidth)
    for(int k = 0;k < xSpace + 2;++k)
      aDstPt[k] = 0;
}

template<class type>
static inline void _dispatch_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  copy_2x2<type>(src,dst,xSpace,ySpace);
  
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  int aNbPixel2Dispatch = (xSpace >> 1) + 1;

  for(int lineId = MAXIPIX_NB_LINE - 1;lineId;--lineId)
    {
      type aPixelValue = *aDstPt / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;

      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;
  
      aDstPt += aTotalWidth - (xSpace + 2);
    }
  

  aDstPt -= MAXIPIX_NB_COLUMN - 1;
  int aNbPixel2DispatchInY = (ySpace >> 1) + 1;
  
  int aPart = 2;
  /* aPart == 2 => upper chip
     aPart == 1 => bottom chip
  */
  do
    {
      type aPixelValue;
      //Last line or first line of left chip (depend on aPart)
      for(int columnIter = MAXIPIX_NB_COLUMN - 1;columnIter;--columnIter,++aDstPt)
	{
	  aPixelValue = *aDstPt / aNbPixel2DispatchInY;
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
  
      //Bottom or top right corner of left chip
      aPixelValue = *aDstPt / (aNbPixel2DispatchInY * aNbPixel2Dispatch);
      for(int columnIter = aNbPixel2Dispatch;columnIter;--columnIter,++aDstPt)
	{
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
      //Bottom or top left corner of right chip
      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / (aNbPixel2DispatchInY * aNbPixel2Dispatch);
      for(int columnIter = aNbPixel2Dispatch;columnIter;--columnIter,++aDstPt)
	{
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
      //Last or first line of right chip
      for(int columnIter = MAXIPIX_NB_COLUMN - 1;columnIter;--columnIter,++aDstPt)
	{
	  aPixelValue = *aDstPt / aNbPixel2DispatchInY;
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aTotalWidth * y] = aPixelValue;
	}
      
      //Set Variable for second part iteration (bottom chip)
      aDstPt += aTotalWidth * ySpace;
      aTotalWidth = -aTotalWidth;
    }
  while(--aPart);

  aDstPt = ((type*)dst->data) + aTotalWidth * (MAXIPIX_NB_LINE + ySpace + 1) +
    MAXIPIX_NB_COLUMN - 1;
  
  for(int lineId = MAXIPIX_NB_LINE - 1;lineId;--lineId)
    {
      type aPixelValue = *aDstPt / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;

      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;
  
      aDstPt += aTotalWidth - (xSpace + 2);
    }
}

template<class type>
static inline void _mean_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  int aTotalWidth = (MAXIPIX_NB_COLUMN * 2) + xSpace;
  copy_2x2<type>(src,dst,xSpace,ySpace);
  type *aDstPt = ((type*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  int aNbXPixel2Dispatch = (xSpace >> 1) + 1;
  int aNbYPixel2Dispatch = (ySpace >> 1) + 1;

  for(int aLineIter = MAXIPIX_NB_LINE - 1;aLineIter;--aLineIter,aDstPt += aTotalWidth)
    {
      float aFirstValue = *aDstPt / float(aNbXPixel2Dispatch);
      float aLastValue = aDstPt[xSpace + 1] / float(aNbXPixel2Dispatch);
      float anInc = (aLastValue - aFirstValue) / (xSpace + 1);
      *aDstPt = (type)aFirstValue;
      aDstPt[xSpace + 1] = (type)aLastValue;
      aFirstValue += anInc;

      for(int i = 0;i < xSpace;++i,aFirstValue += anInc)
	aDstPt[1 + i] = (type)aFirstValue;
    }

  //corner
  int aNbPixel2Dispatch = aNbXPixel2Dispatch + aNbYPixel2Dispatch;
  float a1ftCornerValue = *aDstPt / float(aNbPixel2Dispatch);
  float a2ndCornerValue = aDstPt[xSpace + 1] / float(aNbPixel2Dispatch);
  float a3thCornerValue = aDstPt[(ySpace + 1) * aTotalWidth]  / float(aNbPixel2Dispatch);
  float a4thCornerValue = aDstPt[(ySpace + 1) * aTotalWidth + xSpace + 1] / float(aNbPixel2Dispatch);
  
  float *aFirstLinePt = new float[xSpace +2];
  aFirstLinePt[0] = a1ftCornerValue;
  aFirstLinePt[xSpace + 1] = a2ndCornerValue;
  float anInc = (a2ndCornerValue - a1ftCornerValue) / (xSpace + 1);
  for(int i = 0;i < xSpace;++i)
    aFirstLinePt[i + 1] = aFirstLinePt[i] + anInc;

  float *aLastLinePt = new float[xSpace + 2];
  aLastLinePt[0] = a3thCornerValue;
  aLastLinePt[xSpace + 1] = a4thCornerValue;
  anInc = (a4thCornerValue - a3thCornerValue) / (xSpace + 1);
  for(int i = 0;i < xSpace;++i)
    aLastLinePt[i + 1] = aLastLinePt[i] + anInc;
  
  float *anIncBuffer = new float[xSpace + 2];
  for(int i = 0;i < xSpace + 2;++i)
    anIncBuffer[i] = (aLastLinePt[i] - aFirstLinePt[i]) / (ySpace + 1);
  
  
  for(int lineIter = ySpace + 1;lineIter;--lineIter,aDstPt += aTotalWidth)
    {
      for(int colId = 0;colId < xSpace + 2;++colId)
	{
	  aDstPt[colId] = (type)aFirstLinePt[colId];
	  aFirstLinePt[colId] += anIncBuffer[colId];
	}
    }
  for(int colId = 0;colId < xSpace + 2;++colId)
    aDstPt[colId] = (type)aLastLinePt[colId];

  delete [] aFirstLinePt;
  delete [] aLastLinePt;
  delete [] anIncBuffer;

  //center lines
  aDstPt -= (aTotalWidth * (ySpace + 1)) + MAXIPIX_NB_COLUMN - 1;
  
  type *aFirstValuePt = aDstPt;
  type *aSecondValuePt = aDstPt + (aTotalWidth * (ySpace + 1));
  
  int aNbChip = 2;
  do
    {
      for(int i = MAXIPIX_NB_COLUMN - 1;i;--i,++aFirstValuePt,++aSecondValuePt)
	{
	  float aFirstValue = *aFirstValuePt / float(aNbYPixel2Dispatch);
	  float aLastValue = *aSecondValuePt / float(aNbYPixel2Dispatch);
	  anInc = (aLastValue - aFirstValue) / (ySpace + 1);
	  *aFirstValuePt = (type)aFirstValue;
	  *aSecondValuePt = (type)aLastValue;

	  aFirstValue += anInc;

	  for(int lineIter = 0;lineIter < ySpace;++lineIter,aFirstValue += anInc)
	    aFirstValuePt[(1 + lineIter) * aTotalWidth] = (type)aFirstValue;
	}
      aFirstValuePt += xSpace + 2;
      aSecondValuePt += xSpace + 2;
    }
  while(--aNbChip);

  aDstPt += MAXIPIX_NB_COLUMN - 1 + aTotalWidth;
  for(int aLineIter = MAXIPIX_NB_LINE - 1;aLineIter;--aLineIter,aDstPt += aTotalWidth)
    {
      float aFirstValue = *aDstPt / float(aNbXPixel2Dispatch);
      float aLastValue = aDstPt[xSpace + 1] / float(aNbXPixel2Dispatch);
      float anInc = (aLastValue - aFirstValue) / (xSpace + 1);
      *aDstPt = (type)aFirstValue;
      aDstPt[xSpace + 1] = (type)aLastValue;
      aFirstValue += anInc;

      for(int i = 0;i < xSpace;++i,aFirstValue += anInc)
	aDstPt[1 + i] = (type)aFirstValue;
    }
  
}

MaxipixReconstruction::MaxipixReconstruction(MaxipixReconstruction::Model aModel,
					     MaxipixReconstruction::Type aType) :
  mType(aType),mModel(aModel),mXSpace(4),mYSpace(4)
{
}

MaxipixReconstruction::MaxipixReconstruction(const MaxipixReconstruction &other) :
  mType(other.mType),mModel(other.mModel),
  mXSpace(other.mXSpace),mYSpace(other.mYSpace)
{
}

MaxipixReconstruction::~MaxipixReconstruction()
{
}

void MaxipixReconstruction::setType(MaxipixReconstruction::Type aType)
{
  mType = aType;
}
void MaxipixReconstruction::setModel(MaxipixReconstruction::Model aModel)
{
	mModel = aModel;
}
void MaxipixReconstruction::setXnYGapSpace(int xSpace,int ySpace)
{
  mXSpace = xSpace,mYSpace = ySpace;
}


Data MaxipixReconstruction::process(Data &aData)
{
  Data aReturnData;
  aReturnData = aData;

  if(mModel == M_5x1)
    {
      aReturnData.width = MAXIPIX_NB_COLUMN * 5 + 4 * mXSpace;
      if(!_processingInPlaceFlag)
	{
	  Buffer *aNewBuffer = new Buffer(aReturnData.size());
	  aReturnData.setBuffer(aNewBuffer);
	}
      switch(mType)
	{
	case RAW:
	  _raw_5x1(aData,aReturnData,mXSpace);break;
	case ZERO: 
	  _zero_5x1(aData,aReturnData,mXSpace);break;
	case DISPATCH:
	  _dispatch_5x1(aData,aReturnData,mXSpace);break;
	case MEAN:
	  _mean_5x1(aData,aReturnData,mXSpace);break;
	default:		// ERROR
	  break;
	}
    }
  else			// Model 2x2
    {
      aReturnData.width = MAXIPIX_NB_COLUMN * 2 + mXSpace;
      aReturnData.height = MAXIPIX_NB_LINE * 2 + mYSpace;

      Buffer *aNewBuffer = new Buffer(aReturnData.size());
      switch(mType)
	{
	case RAW:
	  _raw_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);break;
	case ZERO: 
	  _zero_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);break;
	case DISPATCH:
	  _dispatch_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);break;
	case MEAN:
	  _mean_2x2<unsigned short>(aData,aNewBuffer,mXSpace,mYSpace);break;
	default:		// ERROR
	  break;
	}

      if(_processingInPlaceFlag)
	{
	  unsigned char *aSrcPt = (unsigned char*)aNewBuffer->data;
	  unsigned char *aDstPt = (unsigned char*)aData.data();
	  memcpy(aDstPt,aSrcPt,aReturnData.size());
	}
      else
	aReturnData.setBuffer(aNewBuffer);
      aNewBuffer->unref();
    }

  return aReturnData;
}
