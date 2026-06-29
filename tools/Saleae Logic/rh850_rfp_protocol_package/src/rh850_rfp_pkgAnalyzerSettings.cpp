#include "rh850_rfp_pkgAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


rh850_rfp_pkgAnalyzerSettings::rh850_rfp_pkgAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mBitRate( 9600 ),
	mBitRate2( 1000000 ),
	mInputChannelInterface(),
	mBitRateInterface(),
	mBitRate2Interface()
{
	mInputChannelInterface.SetTitleAndTooltip( "Serial", "Standard RH850 RFP (package)" );
	mInputChannelInterface.SetChannel( mInputChannel );

	mBitRateInterface.SetTitleAndTooltip( "Bit Rate (Bits/S)",  "Specify the bit rate in bits per second." );
	mBitRateInterface.SetMax( 6000000 );
	mBitRateInterface.SetMin( 1 );
	mBitRateInterface.SetInteger( mBitRate );

	mBitRate2Interface.SetTitleAndTooltip( "2nd Bit Rate (Bits/S)",  "Specify the bit rate in bits per second." );
	mBitRate2Interface.SetMax( 12000000 );
	mBitRate2Interface.SetMin( 0 );
	mBitRate2Interface.SetInteger( mBitRate2 );

	AddInterface( &mInputChannelInterface );
	AddInterface( &mBitRateInterface );
	AddInterface( &mBitRate2Interface );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "Serial", false );
}

rh850_rfp_pkgAnalyzerSettings::~rh850_rfp_pkgAnalyzerSettings()
{
}

bool rh850_rfp_pkgAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface.GetChannel();
	mBitRate = mBitRateInterface.GetInteger();
	mBitRate2 = mBitRate2Interface.GetInteger();

	ClearChannels();
	AddChannel( mInputChannel, "RH850 RFP (package)", true );

	return true;
}

void rh850_rfp_pkgAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface.SetChannel( mInputChannel );
	mBitRateInterface.SetInteger( mBitRate );
}

void rh850_rfp_pkgAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mBitRate;
	text_archive >> mBitRate2;

	ClearChannels();
	AddChannel( mInputChannel, "RH850 RFP (package)", true );

	UpdateInterfacesFromSettings();
}

const char* rh850_rfp_pkgAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mBitRate;
	text_archive << mBitRate2;

	return SetReturnString( text_archive.GetString() );
}
