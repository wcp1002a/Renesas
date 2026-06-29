#include "rh850_rfp_pkgAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "rh850_rfp_pkgAnalyzer.h"
#include "rh850_rfp_pkgAnalyzerSettings.h"
#include <iostream>
#include <fstream>
#include "rh850_rv40f.h"

rh850_rfp_pkgAnalyzerResults::rh850_rfp_pkgAnalyzerResults( rh850_rfp_pkgAnalyzer* analyzer, rh850_rfp_pkgAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

rh850_rfp_pkgAnalyzerResults::~rh850_rfp_pkgAnalyzerResults()
{
}

void rh850_rfp_pkgAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
	char numberString [128] = "" ;
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	U8  p_cmd;
	U32 p_data;

	// AddResultString( number_str );
	// Package
	switch(frame.mFlags){
	case FRAME_HEADER:
		snprintf (numberString, 128, "COM:%02X", frame.mData1 & 0xFF) ;
		break;
	case FRAME_DATA:
		p_cmd = ((frame.mData1 >> 32) & 0xFF);
		p_data = (frame.mData1 & 0xFFFFFFFF);
		switch(p_cmd){
		case 0x13:
		case 0x15:
		case 0x16:
		case 0x18:
			snprintf (numberString, 128, "CMD%02X, DATA: Start Adr.(4) + End Adr.(4) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x4D:
			snprintf (numberString, 128, "CMD%02X, DATA: Area(1) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x19:
			snprintf (numberString, 128, "CMD%02X, DATA: Sum(4)=H'%08X + SUM:%02X", p_cmd, p_data, frame.mData2) ;
			break;
		case 0x20:
			snprintf (numberString, 128, "CMD%02X, DATA: Security data(1) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x22:
			snprintf (numberString, 128, "CMD%02X, DATA: LB1(1) + LB2(1) + LB3(1) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x26:
			snprintf (numberString, 128, "CMD%02X, DATA: Option-byte (32) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x28:
		case 0x2A:
		case 0x30:
			snprintf (numberString, 128, "CMD%02X, DATA: ID-code (16) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x2D:
			snprintf (numberString, 128, "CMD%02X, DATA: OP1(1) + OP2(1) + OP3(1) + SUM:%02X", p_cmd, frame.mData2) ;
			break;

		case 0x32:
			snprintf (numberString, 128, "CMD%02X, DATA: OSC clock freq.(4) + CPU clock freq.(4) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x34:
			snprintf (numberString, 128, "Set Baud:H'%08X (%ld) + SUM:%02X", p_data, p_data, frame.mData2) ;
			break;
		case 0x74:
		case 0x76:
			snprintf (numberString, 128, "CMD%02X, DATA: ION(1) + IRC(4) + IRD(4) + IRV(4) + UP1(1) + UP2(1) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		case 0x75:
			snprintf (numberString, 128, "CMD%02X, DATA: ICUMOPBT(4) + SUM:%02X", p_cmd, frame.mData2) ;
			break;
		default:
			snprintf (numberString, 128, "CMD%02X, SUM:%02X", p_cmd, frame.mData2) ;
			break;
		}
		break;
	case FRAME_P_BAUDRATE:
		snprintf (numberString, 128, "[Info] Change Baudrate: %d", frame.mData1) ;
		break;
	case FRAME_FOOTER:
		snprintf (numberString, 128, "ETX:%02X", frame.mData1) ;
		break;
	default:
		snprintf (numberString, 128, "%02X", frame.mData1) ;
		break;
	}
	AddResultString( numberString );

	// Command Decode
	if (frame.mFlags == FRAME_HEADER){
		//--------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------


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
			if( commandTable[i].value == (frame.mData1 & 0xFF) ){
				commandName = commandTable[i].name;
				break;
			}
		}

		if( commandName != 0 ){
			snprintf (numberString, 128, "COM:%02X (%s)", (frame.mData1 & 0xFF), commandName);
			AddResultString( numberString );
		}
	}
}

void rh850_rfp_pkgAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
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

void rh850_rfp_pkgAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddTabularText( number_str );
#endif
}

void rh850_rfp_pkgAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void rh850_rfp_pkgAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}