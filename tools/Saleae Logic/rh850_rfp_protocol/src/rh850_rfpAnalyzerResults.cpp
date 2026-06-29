#include "rh850_rfpAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "rh850_rfpAnalyzer.h"
#include "rh850_rfpAnalyzerSettings.h"
#include <iostream>
#include <fstream>
#include "rh850_rv40f.h"

rh850_rfpAnalyzerResults::rh850_rfpAnalyzerResults( rh850_rfpAnalyzer* analyzer, rh850_rfpAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

rh850_rfpAnalyzerResults::~rh850_rfpAnalyzerResults()
{
}

void rh850_rfpAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
	char numberString [128] = "" ;
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

	AddResultString( number_str );

	// Package
	switch(frame.mFlags){
	case BYTE_SOH:
		if (frame.mData1 == 0x81)
			snprintf (numberString, 128, "SOD:%02X", frame.mData1) ;
		else
			snprintf (numberString, 128, "SOH:%02X", frame.mData1) ;
		break;
	case BYTE_LNH:
		snprintf (numberString, 128, "LNH:%02X", frame.mData1) ;
		break;
	case BYTE_LNL:
		snprintf (numberString, 128, "LNL:%02X", frame.mData1) ;
		break;
	case BYTE_COM:
		snprintf (numberString, 128, "COM:%02X", frame.mData1) ;
		break;
	case BYTE_DAT:
		snprintf (numberString, 128, "%02X", frame.mData1) ;
		break;
	case BYTE_SUM:
		snprintf (numberString, 128, "SUM:%02X", frame.mData1, frame.mData2) ;
		break;
	case BYTE_SUM_NG:
		snprintf (numberString, 128, "!SUM:%02X (Expect: %02X)", frame.mData1, (U8) frame.mData2) ;
		break;
	case BYTE_ETX:
		snprintf (numberString, 128, "ETX:%02X", frame.mData1) ;
		break;
	case BYTE_P_BAUD:
		snprintf (numberString, 128, "[Info] Change Baudrate: %d", frame.mData1) ;
		break;
	case BYTE_UNDEFINE:
		snprintf (numberString, 128, "!UND:%02X", frame.mData1) ;
		break;
	defaul:
		snprintf (numberString, 128, "%02X", frame.mData1) ;
		break;
	}

	AddResultString( numberString );

	// Command Decode
	if (frame.mFlags == 4){
		struct CommandEntry {
			U8 value;
			const char* name;
		};
		static const CommandEntry commandTable[] = {
			{ 0x00, "Inquiry" },
			{ 0x10, "Blank Check" },
			{ 0x12, "Erase" },
			{ 0x13, "Write" },
			{ 0x15, "Read" },
			{ 0x16, "Verify" },
			{ 0x17, "ID Verify" },
			{ 0x18, "CRC Sum Check" },
			{ 0x4D, "Addition Sum Check" },
			{ 0x19, "ID Check" },
			{ 0x1C, "Configuration Clear" },

			{ 0x20, "Command protection Set" },
			{ 0x21, "Command protection Get" },
			{ 0x22, "Lock-bit Set" },
			{ 0x23, "Lock-bit Get" },
			{ 0x26, "Option-byte Set" },
			{ 0x27, "Option-byte Get" },
			{ 0x28, "Serial programming ID-code Set" },
			{ 0x29, "Serial programming Disable" },
			{ 0x2A, "ID-code Set" },
			{ 0x2B, "ID-code Get" },
			{ 0x2C, "ID authentication mode Get" },
			{ 0x2D, "OTP Set" },
			{ 0x2E, "OTP Get" },

			{ 0x30, "Serial programming ID-code Check" },
			{ 0x32, "Frequency Set" },
			{ 0x34, "Baud Rate Set" },
			{ 0x38, "Device Type Get" },
			{ 0x3A, "Signature Get" },

			{ 0x74, "ICU-M configuration set" },
			{ 0x75, "ICU-M option byte set" },
			{ 0x76, "ICU-M verify" },
			{ 0x77, "ICU-M mode get" },
		};

		const char* commandName = 0;
		for( U32 i = 0; i < sizeof(commandTable) / sizeof(commandTable[0]); i++ ){
			if( commandTable[i].value == frame.mData1 ){
				commandName = commandTable[i].name;
				break;
			}
		}

		if( commandName != 0 ){
			snprintf (numberString, 128, "COM:%02X (%s)", frame.mData1, commandName);
			AddResultString( numberString );
		}
	}
}

void rh850_rfpAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void rh850_rfpAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddTabularText( number_str );
#endif
}

void rh850_rfpAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void rh850_rfpAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}