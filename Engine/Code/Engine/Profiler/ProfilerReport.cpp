#include "Engine/Profiler/ProfilerReport.hpp"


void ProfilerReport::GenerateFlatReportFromFrame( ProfilerMeasurement* rootMeasurement ) {
	root = new ProfilerReportEntry();
	root->id = rootMeasurement->id;
	root->AccumulateData(rootMeasurement);
	root->PopulateFlat( rootMeasurement, root );
	Finish();


}


void ProfilerReport::GenerateTreeReportFromFrame( ProfilerMeasurement* rootMeasurement ) {
	root = new ProfilerReportEntry();
	root->id = rootMeasurement->id;
	root->PopulateTree( rootMeasurement );
	Finish();
}


void ProfilerReport::Finish() {
	root->Finish(root);
}


double ProfilerReport::GetTotalFrameTime() {
	return root->totalTime;
}