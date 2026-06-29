#ifndef RH850_RFP_ANALYZER_SETTINGS
#define RH850_RFP_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class rh850_rfpAnalyzerSettings : public AnalyzerSettings
{
public:
	rh850_rfpAnalyzerSettings();
	virtual ~rh850_rfpAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mBitRate;
	U32 mBitRate2;

protected:
	AnalyzerSettingInterfaceChannel	mInputChannelInterface;
	AnalyzerSettingInterfaceInteger	mBitRateInterface;
	AnalyzerSettingInterfaceInteger	mBitRate2Interface;
};

#endif //RH850_RFP_ANALYZER_SETTINGS
