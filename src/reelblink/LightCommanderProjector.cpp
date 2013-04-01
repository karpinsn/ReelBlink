#include "LightCommanderProjector.h"

LightCommanderProjector::LightCommanderProjector(void)
{ }


LightCommanderProjector::~LightCommanderProjector(void)
{ }

bool LightCommanderProjector::Init( void )
{
  Byte logLevel = 0;
  
  if(!_CheckLogError(InitPortabilityLayer(logLevel+1, logLevel, LightCommanderCCallback)))
  {
		// We have a problem initializing. Just log and be done
		cout << "Unable to initalize connection to the projector\n";
		return false;
  }

	UInt16 order[1];
	order[0] = 0;
	if ( !_CheckLogError( DLP_RegIO_WriteImageOrderLut(1, order, 1) ) )
	{
		cout << "Unable to specify image order\n";
		return false;
	}

	if ( !_CheckLogError( DLP_Source_SetDataSource( SL_SW ) ) )
	{
		cout << "Unable to set the data source\n";
		return false;
	}

  return true;
}

bool LightCommanderProjector::ProjectImage(cv::Mat image)
{
  // Check the image and make sure it is the right size
  if( image.cols != GetWidth() || image.rows != GetHeight() )
  {
	cout << "Incorrect image specified. Check size and channel count\n";
	return false;
  }

  auto binaryImage = _Convert2BinaryImage( image );
  auto byteCount = GetWidth() * GetHeight() / 8;

  if ( !_CheckLogError( DLP_Img_DownloadBitplanePatternToExtMem( binaryImage.get(), byteCount, 0 ) ) )
  {
	cout << "Unable to transfer images to the projector\n";
	return false;
  }

  if( !_CheckLogError( DLP_Display_DisplayPatternManualForceFirstPattern( ) ) )
  {
	cout << "Unable to display the first pattern\n";
	return false;
  }

	if( !_CheckLogError( DLP_Display_DisplayPatternAutoStepRepeatForMultiplePasses( ) ) )
	{
			cout << "Unable to project pattern\n";
			return false;
	}

  return true;
}

int LightCommanderProjector::GetWidth()
{
  // According to the documentation the 
  // LightCommander will only ever project this size
  return 1024;
}

int LightCommanderProjector::GetHeight()
{
  // According to the documentation the 
  // LightCommander will only ever project this size
  return 768;
}

unique_ptr<Byte[]> LightCommanderProjector::_Convert2BinaryImage(cv::Mat byteImage)
{
  int binaryImageSize = GetWidth() * GetHeight() / 8;
  auto binaryImage = unique_ptr<Byte[]>( new Byte[binaryImageSize] );

  Byte packedByte = 0;
  int currentByteIndex = 0;
  int currentBitIndex = 0;
  
  for(int row = 0; row < byteImage.rows; ++row)
  {
	for(int col = 0; col < byteImage.cols; ++col)
	{
	  packedByte = byteImage.at<uchar>(row, col) >= 128 ?
		packedByte | MSB_HIGH : packedByte & MSB_LOW;
	  
	  packedByte = packedByte >> 1;
	  currentBitIndex = (currentBitIndex + 1) % 8;

	  if( !currentBitIndex )
	  {
	    binaryImage[currentByteIndex] = packedByte; 
		currentByteIndex++;
	  }
	}
  }

  return binaryImage;
}

bool LightCommanderProjector::_CheckLogError(Status status)
{
  if(status != STAT_OK)
	{ return false; } // Log our error and return false

  return true;
}

void LightCommanderCCallback(String loggingCallback)
{
  // TODO - Do nothing for right now. If we need, we can callback to the class
  // using a static function. This is just weird C stuff
  cout << loggingCallback;
}