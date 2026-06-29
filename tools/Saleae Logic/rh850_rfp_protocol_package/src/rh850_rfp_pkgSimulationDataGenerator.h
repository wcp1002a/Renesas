#ifndef RH850_RFP_PKG_SIMULATION_DATA_GENERATOR
#define RH850_RFP_PKG_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class rh850_rfp_pkgAnalyzerSettings;

class rh850_rfp_pkgSimulationDataGenerator
{
public:
	rh850_rfp_pkgSimulationDataGenerator();
	~rh850_rfp_pkgSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, rh850_rfp_pkgAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	rh850_rfp_pkgAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //RH850_RFP_PKG_SIMULATION_DATA_GENERATOR