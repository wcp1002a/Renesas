#ifndef RH850_RFP_ANALYZER_RESULTS
#define RH850_RFP_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class rh850_rfpAnalyzer;
class rh850_rfpAnalyzerSettings;

class rh850_rfpAnalyzerResults : public AnalyzerResults
{
public:
	rh850_rfpAnalyzerResults( rh850_rfpAnalyzer* analyzer, rh850_rfpAnalyzerSettings* settings );
	virtual ~rh850_rfpAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	rh850_rfpAnalyzerSettings* mSettings;
	rh850_rfpAnalyzer* mAnalyzer;
};

#endif //RH850_RFP_ANALYZER_RESULTS
