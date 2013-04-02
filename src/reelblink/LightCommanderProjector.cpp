#include "LightCommanderProjector.h"

LightCommanderProjector::LightCommanderProjector(void)
{ }


LightCommanderProjector::~LightCommanderProjector(void)
{ }

bool LightCommanderProjector::Init( void )
{
  Byte logLevel = 0;
  
  // Init communications with the projector
  if(!_CheckLogError(InitPortabilityLayer(logLevel+1, logLevel, LightCommanderCCallback)))
  {
	// We have a problem initializing. Just log and be done
	cout << "Unable to initalize connection to the projector\n";
	return false;
  }

  if( !_WritePlayRegister( ) )
  {
	cout << "Unable to initialize the projector registers\n";
	return false;
  }

  // Disable flash compile. We will just send all commands over USB/SPI
  if( !_CheckLogError( DLP_FlashCompile_SetCompileMode(false) ) )
  {
	cout << "Can't disable flash compile mode\n";
	return false;
  }

  UInt16 order[1];
  order[0] = 0;
  if ( !_CheckLogError( DLP_RegIO_WriteImageOrderLut(1, order, 1) ) )
  {
	cout << "Unable to specify image order\n";
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

  if( !SetLEDIntensity(100, 100, 100, 100) )
  {
	cout << "Unable to set LEDs\n";
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

bool LightCommanderProjector::SetLEDIntensity(float r, float g, float b, float ir)
{
  if(!_SetLED(LED_R, r) || !_SetLED(LED_G, g) || !_SetLED(LED_B, b) || !_SetLED(LED_IR, ir) )
  {
	cout << "Unable to set the LED intensity\n";
	return false;
  }

  return true;
}

bool LightCommanderProjector::_SetLED(LED_t led, float intensity)
{
  if( intensity > 0.0 )
  {
	if ( _CheckLogError( DLP_LED_SetLEDEnable(led, 1) ) )
	  { return _CheckLogError( DLP_LED_SetLEDintensity(led, min(intensity, 100.0f)) ); }
	else
	  { return false; }
  }

  return _CheckLogError( DLP_LED_SetLEDEnable(led, 0) );
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

bool LightCommanderProjector::_WritePlayRegister( void )
{
  DLP_CHECKRETURNBATCHCOMMAND("ExecutePassthroughDLPAPI  _DisablePwmSeq")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x000000c0")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00000008")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00260005")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00080004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00680004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xa030000a")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xffff0003")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x752e0004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00300004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00000001")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00180005")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00f9f004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x0e669004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x002f9008")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xc78e9f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xc78e9f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xc78e9f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xc78e9f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xf9908f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xf9908f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0xf9908f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x0f900f04")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00180004")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x1111, 0x00000001")
  DLP_CHECKRETURNBATCHCOMMAND("DLP_RegIO_EndLUTdata SEQ_LUT")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x2B8, 0x00003f0f")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0xC0, 0x00000044")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x4C4, 0x00196e6a")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0xCC4, 0x00000000")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0xCD0, 0x000003c0")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0xCC8, 0x00000001")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0xCCC, 0x00000001")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0xCD8, 0x00000105")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0x500, 0x00000000")
  DLP_CHECKRETURNBATCHCOMMAND("WriteReg 0xCD4, 0x00006000")
  DLP_CHECKRETURNBATCHCOMMAND("ExecutePassthroughDLPAPI  _EnablePwmSeq")
  DLP_CHECKRETURNBATCHCOMMAND("DLP_Source_SetDataSource  6")

  return true;
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
  cout << loggingCallback << endl;
}