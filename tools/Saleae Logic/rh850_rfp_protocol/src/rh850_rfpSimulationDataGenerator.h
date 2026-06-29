#ifndef RH850_RFP_SIMULATION_DATA_GENERATOR
#define RH850_RFP_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class rh850_rfpAnalyzerSettings;

class rh850_rfpSimulationDataGenerator
{
public:
	rh850_rfpSimulationDataGenerator();
	~rh850_rfpSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, rh850_rfpAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	rh850_rfpAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //RH850_RFP_SIMULATION_DATA_GENERATOR