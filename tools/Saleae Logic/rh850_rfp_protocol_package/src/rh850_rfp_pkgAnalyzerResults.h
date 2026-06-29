#ifndef RH850_RFP_PKG_ANALYZER_RESULTS
#define RH850_RFP_PKG_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class rh850_rfp_pkgAnalyzer;
class rh850_rfp_pkgAnalyzerSettings;

class rh850_rfp_pkgAnalyzerResults : public AnalyzerResults
{
public:
	rh850_rfp_pkgAnalyzerResults( rh850_rfp_pkgAnalyzer* analyzer, rh850_rfp_pkgAnalyzerSettings* settings );
	virtual ~rh850_rfp_pkgAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	rh850_rfp_pkgAnalyzerSettings* mSettings;
	rh850_rfp_pkgAnalyzer* mAnalyzer;
};

#endif //RH850_RFP_PKG_ANALYZER_RESULTS
