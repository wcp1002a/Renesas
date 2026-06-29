#include "rh850_rfp_pkgAnalyzer.h"
#include "rh850_rfp_pkgAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include "rh850_rv40f.h"

rh850_rfp_pkgAnalyzer::rh850_rfp_pkgAnalyzer()
:	Analyzer2(),  
	mSettings(),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
}

rh850_rfp_pkgAnalyzer::~rh850_rfp_pkgAnalyzer()
{
	KillThread();
}

void rh850_rfp_pkgAnalyzer::SetupResults()
{
	// SetupResults is called each time the analyzer is run. Because the same instance can be used for multiple runs, we need to clear the results each time.
	mResults.reset(new rh850_rfp_pkgAnalyzerResults( this, &mSettings ));
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings.mInputChannel );
}

void rh850_rfp_pkgAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();

	mSerial = GetAnalyzerChannelData( mSettings.mInputChannel );

	if( mSerial->GetBitState() == BIT_LOW )
		mSerial->AdvanceToNextEdge();

	U32 samples_per_bit = mSampleRateHz / mSettings.mBitRate;
	U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings.mBitRate ) );
	//------------------------------
	U8  flg_change_baud2 = 0;
	U8  p_bytes_count = 0;
	U8  p_cmd = 0;
	U8  p_sum = 0;
	U8  frame_end[5];
	U8  frame_type = FRAME_HEADER;
	U16 pLength = 0;
	U32 p_baudrate = 0;
	U64 p_st = 0;
	U64 pData1 = 0;
	U64 pData2 = 0;
	//------------------------------

	for( ; ; )
	{
		U8 data = 0;
		U8 mask = 1;
		U8 package_datas[32];

		mSerial->AdvanceToNextEdge(); //falling edge -- beginning of the start bit

		//--------------------------------------------------------------------------------
		// Switch baudrate when 0x34 command detected
		if(flg_change_baud2 == 1){
			if(mSettings.mBitRate2 == 0){
				if(p_baudrate == 0)
					p_baudrate = mSettings.mBitRate2;
				// Use 0x34 command setting
				samples_per_bit = mSampleRateHz / p_baudrate;
				samples_to_first_center_of_first_data_bit = U32( 1.5 * double(samples_per_bit));

			}else{
				// Use setting
				samples_per_bit = mSampleRateHz / mSettings.mBitRate2;
				samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings.mBitRate2) );
				p_baudrate = mSettings.mBitRate2;
			}
			// FRAME_P_BAUDRATE
			Frame frame;
			frame.mData1 = p_baudrate;
			// frame.mData2 = p_bytes_count;
			frame.mFlags = FRAME_P_BAUDRATE;
			frame.mStartingSampleInclusive = mSerial->GetSampleNumber() - 11 * samples_per_bit;
			frame.mEndingSampleInclusive = mSerial->GetSampleNumber() - samples_per_bit;

			mResults->AddFrame( frame );

			mResults->AddMarker( mSerial->GetSampleNumber() - samples_per_bit / 2, AnalyzerResults::UpArrow, mSettings.mInputChannel );
			flg_change_baud2 = 0;
		}
		//--------------------------------------------------------------------------------
		U64 starting_sample = mSerial->GetSampleNumber();

		mSerial->Advance( samples_to_first_center_of_first_data_bit );

		for( U32 i=0; i<8; i++ )
		{
			//let's put a dot exactly where we sample this bit:
			mResults->AddMarker( mSerial->GetSampleNumber(), AnalyzerResults::Dot, mSettings.mInputChannel );

			if( mSerial->GetBitState() == BIT_HIGH )
				data |= mask;

			mSerial->Advance( samples_per_bit );

			mask = mask << 1;
		}

		p_bytes_count++;
		package_datas[p_bytes_count] = data;
		//--------------------------------------------------------------------------------
		// Package
		switch(frame_type){
		case FRAME_HEADER:
			pData1 = (pData1 << 8) | data;

			if (p_bytes_count == 1){
				p_st = starting_sample;
				if(data==0x55){
					frame_end[FRAME_HEADER] = 1;
				}
			}else if(p_bytes_count == 3){
				// SOH, LNH, LNL + CMD
				pLength = (package_datas[2]<<8) | package_datas[3];
				if (pLength == 0){
					frame_end[FRAME_HEADER] = 3;
				}else{
					frame_end[FRAME_HEADER] = 4;
				}
			}else if(p_bytes_count == 4){
				p_cmd = (U8) data;
			}
			break;
	    case FRAME_DATA:
	    	if (p_bytes_count == frame_end[FRAME_HEADER]+1){
				p_st = starting_sample;
	    		frame_end[FRAME_DATA] = frame_end[FRAME_HEADER] + pLength;
	    	}
	    	if(p_bytes_count <= 4 + 4){
	    		// Only store first 4 bytes
				pData1 = (pData1 << 8) | data;
	    	}
	    	// Calculate sum
			if (p_bytes_count == frame_end[FRAME_DATA]){
				// Store CMD value in higher 32 bits.
				pData1 = ((U64)package_datas[4] << 32) | pData1;
				p_sum = 0;
				for(U8 i=2; i<frame_end[FRAME_DATA]; i++)
					p_sum += package_datas[i];
				pData2 = (U8)(0x100 - p_sum);
			}
	    	break;
	    case FRAME_FOOTER:
	    	if(p_bytes_count == frame_end[FRAME_DATA]+1){
				p_st = starting_sample;
	    		frame_end[FRAME_FOOTER] = frame_end[FRAME_DATA] + 1;
				pData1 = (pData1 << 8) | data;
	    	}
	    	break;
		}

		if (p_bytes_count == frame_end[frame_type]){
			Frame frame;
			frame.mData1 = pData1;
			frame.mData2 = pData2;
			frame.mFlags = frame_type;
			frame.mStartingSampleInclusive = p_st;
			frame.mEndingSampleInclusive = mSerial->GetSampleNumber();

			mResults->AddFrame( frame );
			mResults->CommitResults();
			ReportProgress( frame.mEndingSampleInclusive );
			
			pData1 = 0;
			frame_type++;
			if(frame_type == FRAME_UNDEFINE)
				frame_type = FRAME_HEADER;
			// if(frame_type == FRAME_HEADER)
			// 	frame_type = FRAME_DATA;
			// else if(frame_type == FRAME_DATA)
			// 	frame_type = FRAME_FOOTER;
			// else
			// 	frame_type = FRAME_HEADER;
		}

		//--------------------------------------------------------------------------------

		//we have a byte to save. 
		// Frame frame;
		// frame.mData1 = data;
		// frame.mFlags = p_bytes_count;
		// frame.mStartingSampleInclusive = starting_sample;
		// frame.mEndingSampleInclusive = mSerial->GetSampleNumber();

		// mResults->AddFrame( frame );
		// mResults->CommitResults();
		// ReportProgress( frame.mEndingSampleInclusive );

		// paul package detection
		if( mSerial->GetBitState() == BIT_LOW )
			mSerial->AdvanceToNextEdge();

		U64 p_cur = mSerial->GetSampleNumber();
		U64 p_next = mSerial->GetSampleOfNextEdge();
		if((p_next - p_cur) > 10 * samples_per_bit){
			p_bytes_count = 0;
			pData1 = 0;
			pData2 = 0;
			pLength = 0;
			frame_type = FRAME_HEADER;
			frame_end[FRAME_HEADER] = 4;
			if(p_cmd == 0x34){
				U64 p_width = mSerial->GetSampleNumber() - starting_sample;
				p_baudrate = 0;
				p_baudrate |= (package_datas[5] << 24);
				p_baudrate |= (package_datas[6] << 16);
				p_baudrate |= (package_datas[7] << 8);
				p_baudrate |= (package_datas[8] << 0);
				flg_change_baud2 = 1;
			}
		}
	}
}

bool rh850_rfp_pkgAnalyzer::NeedsRerun()
{
	return false;
}

U32 rh850_rfp_pkgAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 rh850_rfp_pkgAnalyzer::GetMinimumSampleRateHz()
{
	return mSettings.mBitRate * 4;
}

const char* rh850_rfp_pkgAnalyzer::GetAnalyzerName() const
{
	return "RH850 RFP (package)";
}

const char* GetAnalyzerName()
{
	return "RH850 RFP (package)";
}

Analyzer* CreateAnalyzer()
{
	return new rh850_rfp_pkgAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}