#include "Engine/Profiler/ProfilerReportEntry.hpp"

void ProfilerReportEntry::PopulateTree( ProfilerMeasurement* measurement ) {
	
	AccumulateData( measurement );
	for (unsigned int index = 0; index < measurement->children.size(); index++) {
		ProfilerMeasurement* currentChild = measurement->children[index];
		ProfilerReportEntry* entry = GetOrCreateChild( currentChild->id );
		entry->PopulateTree(currentChild);
	}
} 


void ProfilerReportEntry::PopulateFlat( ProfilerMeasurement* measurement, ProfilerReportEntry* root ) {
	
	for (unsigned int index = 0; index < measurement->children.size(); index++) {
		ProfilerMeasurement* currentChild = measurement->children[index];
		ProfilerReportEntry* entry = root->GetOrCreateChild( currentChild->id );
		entry->AccumulateData( currentChild );
		PopulateFlat(currentChild, root);
	}
}


void ProfilerReportEntry::AccumulateData( ProfilerMeasurement* measurement ) {
	callCount++;
	totalTime += measurement->GetLengthSeconds();
}


ProfilerReportEntry* ProfilerReportEntry::GetOrCreateChild( const std::string& childID ) {
	std::map<std::string, ProfilerReportEntry*>::iterator searchResult = children.find(childID);
	if (searchResult != children.end()) {
		return searchResult->second;
	}
	else {
		ProfilerReportEntry* entry = new ProfilerReportEntry();
		entry->id = childID;
		entry->parent = this;
		children[childID] = entry;
		return entry;
	}
}


void ProfilerReportEntry::Finish( ProfilerReportEntry* root ) {

	double childrenTime = 0.0;
	std::map<std::string, ProfilerReportEntry*>::iterator it = children.begin();
	while (it != children.end()) {
		it->second->Finish( root );
		childrenTime += it->second->totalTime;
		it++;
	}

	selfTime = totalTime - childrenTime;
	selfPercentTime = selfTime / root->totalTime;
	percentTime = totalTime / root->totalTime;
}