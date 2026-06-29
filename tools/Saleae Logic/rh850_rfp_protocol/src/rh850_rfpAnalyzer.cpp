#include "rh850_rfpAnalyzer.h"
#include "rh850_rfpAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include "rh850_rv40f.h"

rh850_rfpAnalyzer::rh850_rfpAnalyzer()
:	Analyzer2(),  
	mSettings(),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
}

rh850_rfpAnalyzer::~rh850_rfpAnalyzer()
{
	KillThread();
}

void rh850_rfpAnalyzer::SetupResults()
{
	// SetupResults is called each time the analyzer is run. Because the same instance can be used for multiple runs, we need to clear the results each time.
	mResults.reset(new rh850_rfpAnalyzerResults( this, &mSettings ));
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings.mInputChannel );
}

void rh850_rfpAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();

	mSerial = GetAnalyzerChannelData( mSettings.mInputChannel );

	if( mSerial->GetBitState() == BIT_LOW )
		mSerial->AdvanceToNextEdge();

	U32 samples_per_bit = mSampleRateHz / mSettings.mBitRate;
	U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings.mBitRate ) );
	U8  flg_change_baud2 = 0;
	U8  p_bytes_count = 0;
	U8  p_cmd = 0;
	U8  p_sum = 0;
	U8  package_datas[128];
	U16 p_length = 0;
	U32 p_baudrate = 0;

	for( ; ; )
	{
		U8 data = 0;
		// U8 mask = 1 << 7;
		U8 mask = 1;
		
		mSerial->AdvanceToNextEdge(); //falling edge -- beginning of the start bit

		//--------------------------------------------------------------------------------
		// Switch baudrate when 0x34 command detected
		if(flg_change_baud2 == 1){
			if(mSettings.mBitRate2 == 0){
				// Use 0x34 command setting
				samples_per_bit = mSampleRateHz / p_baudrate;
				samples_to_first_center_of_first_data_bit = U32( 1.5 * double(samples_per_bit));

			}else{
				// Use setting
				samples_per_bit = mSampleRateHz / mSettings.mBitRate2;
				samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings.mBitRate2) );
			}
			// BYTE_P_BAUD
			Frame frame;
			frame.mData1 = mSettings.mBitRate2;
			// frame.mData2 = p_bytes_count;
			frame.mFlags = BYTE_P_BAUD;
			frame.mStartingSampleInclusive = mSerial->GetSampleNumber() - 11 * samples_per_bit;
			frame.mEndingSampleInclusive = mSerial->GetSampleNumber() - samples_per_bit;

			mResults->AddFrame( frame );

			mResults->AddMarker( mSerial->GetSampleNumber() - samples_per_bit / 2, AnalyzerResults::UpArrow, mSettings.mInputChannel );
			flg_change_baud2 = 0;
		}
		// if(flg_change_baud == 0){
		// 	U64 p_cur = mSerial->GetSampleNumber();
		// 	U64 p_next = mSerial->GetSampleOfNextEdge();

		// 	// if((p_next - p_cur) < (mSampleRateHz / (100000))){
		// 	// The data is 0x01 or 0x81, it means the first pulse width is same as one bit.
		// 	// Filter out excessively high frequency noise (2MHz)
		// 	if(((p_next - p_cur) < (samples_per_bit / 2)) && ((p_next - p_cur ) > (mSampleRateHz / 2000000))){ // only half of a bit
		// 		flg_change_baud = 1;
		// 		// samples_per_bit = mSampleRateHz / 1066000;
		// 		// samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( 1066000 ) );
		// 		if(mSettings.mBitRate2 == 1){
		// 			// Auto detect
		// 			samples_per_bit = (p_next - p_cur);	// one bit samples
		// 			samples_to_first_center_of_first_data_bit = U32( 1.5 * double(samples_per_bit));	// 1.5 bit
		// 		}else{
		// 			// Use setting
		// 			samples_per_bit = mSampleRateHz / mSettings.mBitRate2;
		// 			samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings.mBitRate2) );
		// 		}
		// 		mResults->AddMarker( p_cur+samples_to_first_center_of_first_data_bit/3, AnalyzerResults::UpArrow, mSettings.mInputChannel );
		// 	}
		// }
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

			// mask = mask >> 1;
			mask = mask << 1;
		}


		p_bytes_count++;
		package_datas[p_bytes_count] = data;
		//--------------------------------------------------------------------------------
		if(p_bytes_count == 3){
			p_length = (package_datas[2]<<8) | package_datas[3];
		}else if(p_bytes_count == 4){
			p_cmd = (U8) data;
		}
		if(p_bytes_count > 1){
			p_sum += (U8) data;
		}
		//--------------------------------------------------------------------------------
		//we have a byte to save. 
		Frame frame;
		frame.mData1 = data;
		frame.mData2 = p_bytes_count;
		if (p_bytes_count <= 4){
			frame.mFlags = p_bytes_count;
		}else if(p_bytes_count == p_length + 4) {
			frame.mFlags = BYTE_SUM;
			if(p_sum != 0x00){
				frame.mFlags = BYTE_SUM_NG;
				frame.mData2 = 0x100 + data - p_sum;
			}
		}else if(p_bytes_count == p_length + 5) {
			frame.mFlags = BYTE_ETX;
		}else{
			frame.mFlags = BYTE_DAT;
		}
		frame.mStartingSampleInclusive = starting_sample;
		frame.mEndingSampleInclusive = mSerial->GetSampleNumber();

		mResults->AddFrame( frame );
		//--------------------------------------------------------------------------------
		// FrameV2
		//--------------------------------------------------------------------------------
		FrameV2 frameV2 ;
		frameV2.AddInteger("p_bytes_count", p_bytes_count);
		frameV2.AddInteger("data", data);
      	mResults->AddFrameV2 (frameV2, "result_type", starting_sample, mSerial->GetSampleNumber()) ;


		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );

		// paul
		if( mSerial->GetBitState() == BIT_LOW )
			mSerial->AdvanceToNextEdge();

		U64 p_cur = mSerial->GetSampleNumber();
		U64 p_next = mSerial->GetSampleOfNextEdge();
		if((p_next - p_cur) > 10 * samples_per_bit){
			p_bytes_count = 0;
			p_sum = 0;
			if(p_cmd == 0x34){
				U64 p_width = mSerial->GetSampleNumber() - starting_sample;
				p_baudrate = 0;
				p_baudrate |= (package_datas[5] << 24);
				p_baudrate |= (package_datas[6] << 16);
				p_baudrate |= (package_datas[7] << 8);
				p_baudrate |= (package_datas[8] << 0);
				flg_change_baud2 = 1;
				// // BYTE_P_BAUD
				// Frame frame;
				// frame.mData1 = p_baudrate;
				// frame.mData2 = p_bytes_count;
				// frame.mFlags = BYTE_P_BAUD;
				// frame.mStartingSampleInclusive = mSerial->GetSampleNumber();
				// frame.mEndingSampleInclusive = mSerial->GetSampleNumber() + p_width;

				// mResults->AddFrame( frame );
			}
		}

	}
}

bool rh850_rfpAnalyzer::NeedsRerun()
{
	return false;
}

U32 rh850_rfpAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 rh850_rfpAnalyzer::GetMinimumSampleRateHz()
{
	return mSettings.mBitRate * 4;
}

const char* rh850_rfpAnalyzer::GetAnalyzerName() const
{
	return "RH850 RFP";
}

const char* GetAnalyzerName()
{
	return "RH850 RFP";
}

Analyzer* CreateAnalyzer()
{
	return new rh850_rfpAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}