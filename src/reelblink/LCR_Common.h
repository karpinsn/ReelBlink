#ifndef _LCR_Common_H_
#define _LCR_Common_H_

// for vs
typedef unsigned int uint;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;

#define HEADER_SIZE			6
#define MAX_PACKET_SIZE		0x7FF7
#define MAX_PAYLOAD_SIZE    0x7FF0
#define CHECKSUM_SIZE		1



typedef enum 
{
	Success = 0x00,
	UnknownError = 0x01,
	InvalidCommand = 0x02,
	InvalidParameter = 0x03,
	OutofMemory = 0x04,	
	HardwareFailure = 0x05,
	HardwareBusy = 0x06,
	NotInitlized = 0x07,
	ReferenceError = 0x08,
	CheckSumError = 0x09,
	PacketFormatError= 0x0A,
	CommandContinuation = 0x0B

} LCR_Byte_Zero_Packet;

typedef enum
{
	LCR_System_Busy = 0x00,
	LCR_Error = 0x01,
	Host_Write = 0x02,
	LCR_Write_Response = 0x03,
	Host_Read_Command = 0x04,
	LCR_ReponsePacket = 0x05

} Packet_Type;




typedef enum
{
	VersionString = 0x0100,
	CurrentDisplayMode = 0x0101,
	CurrentTestPattern = 0x0103,
	LedCurrentSettings = 0x0104,
	StaticImage = 0x0105,
	StaticColor = 0x0106,
	DisplaySetting = 0x0107,
	VideoInput = 0x0200,
	VideoMode = 0x0201,
	PatternSequencing = 0x0400,
	PatternDefinition = 0x0401,
	StatPatternSequence = 0x0402,
	AdvancedPatternSequence = 0x0403,
	TriggerOutputSetting = 0x0404,
	DisplayPattern = 0x0405,
	ExtendedPatternSequence = 0x0480,
	ExtendedPatternDefinition = 0x0481,
	CameraCapture = 0x0500,
	SaveReadSolution = 0x0600,
	ManageSolution = 0x0601,
	InstallationFile = 0x0700,
	SetIpAddress = 0x0800,
	DPLC3000Register = 0xFF00
} CommandIds;

typedef enum
{
	SW_Revision = 0x00,
	FPGA_Firmware = 0x10,
	MSP430 = 0x20
	
} LCR_Revision;


typedef enum
{
	DataComplete = 0x00,
	Beginning = 0x01,
	Intermediate = 0x02,
	End = 0x03

}Command_Flags;


typedef enum
{
	StaticImageMode = 0x00,
	InternalTestPatter = 0x01,
	HdmiVideoInput = 0x02,
	Reserved = 0x03,
	PatternSequenceDisplay = 0x04

} DisplayMode;

#endif