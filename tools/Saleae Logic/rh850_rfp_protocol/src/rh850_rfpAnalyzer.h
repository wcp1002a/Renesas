#ifndef RH850_RFP_ANALYZER_H
#define RH850_RFP_ANALYZER_H

#include <Analyzer.h>
#include "rh850_rfpAnalyzerSettings.h"
#include "rh850_rfpAnalyzerResults.h"
#include "rh850_rfpSimulationDataGenerator.h"
#include <memory>

class ANALYZER_EXPORT rh850_rfpAnalyzer : public Analyzer2
{
public:
	rh850_rfpAnalyzer();
	virtual ~rh850_rfpAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	rh850_rfpAnalyzerSettings mSettings;
	std::unique_ptr<rh850_rfpAnalyzerResults> mResults;
	AnalyzerChannelData* mSerial;

	rh850_rfpSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //RH850_RFP_ANALYZER_H
