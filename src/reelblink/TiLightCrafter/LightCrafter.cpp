#include "LightCrafter.h"


LightCrafter::LightCrafter()
{
    Commander = unique_ptr<LCR_Commander>(new LCR_Commander());
	IsConnected = false;
}

int LightCrafter::GetHeight(void)
{
  //static image has to be a bmp at 608 x 684
  return 684;
}

int LightCrafter::GetWidth(void)
{
  //static image has to be a bmp at 608 x 684
  return 608;
}

bool LightCrafter::Connect()
{
    bool connected = Commander->Connect_LCR(LCR_Default_IP,LCR_Default_PORT);

	if(!connected)
	{
	  cout<<"Cannot Connect to LCR.\n";
	}
	else
	{
	  cout<<"Connected To LCR.\n";
	  IsConnected = true;
	}
	return connected;
}

bool LightCrafter::Disconnect()
{
    bool disconnected = Commander ->Disconnect_LCR();

	if(!disconnected)
	{
	  cout<< "Could not disconnect from the LCR.\n";
	}
	else
	{
	  cout<<"Disconnected from LCR.\n";
	  IsConnected = false;
	}

	return disconnected;
}

bool LightCrafter::StaticDisplayMode()
{
    DisplayMode displayMode= StaticImageMode;
	bool modeChanged = Commander -> SetDisplayMode(displayMode);
	
	if(!modeChanged)
	{
	  cout<<"Could not change display mode to Static.\n";
	}
	else
	{
	  cout<<"Mode changed to Static Display.\n";
	}
	  
	return modeChanged;
}


bool LightCrafter::ProjectImage(cv::Mat image)
{
    CvMat* imageStream =  BitmapCreator::CreateBitmapFromMat(image);
	bool imageLoaded  = Commander ->LCR_LOAD_STATIC_IMAGE( imageStream->data.ptr, imageStream->step);

	if(!imageLoaded)
	{
	  cout<<"Could not load static 608 x 684 24bit static bmp.\n";
	}
	else
	{
	  cout<<"Static Image Loaded.\n";
	}
	 
	// everthing went smoothly

	return imageLoaded;
}



