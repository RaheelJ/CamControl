#pragma once

#include "../include/Auxiliary.h"

class CoverageBlock
{
	std::string region_priority_method;
	std::vector<_queue_element> PriorityQueue;
	std::vector<_region_interest> RegionsOfInterest;
	_priority_weights Weights;
	double region_share{ 0 };
	int operation_mode{ 0 };
	double current_time{ 0 };
	double scan_time{ 0 };							//Period after which scan should be performed in search and track mode

	bool NormalizeTargets(const double);
	bool NormalizeRegions(const double);
	bool NormalizeQueue();
	bool UpdateQueue();								//Add or update region of interest in the priority queue

	double CalcPriority(const _target&);			//Calculate priority for the target
	double CalcPriority(const _region_interest&);	//Calculate priority for the region of interest
	
	public:
		bool Initialize(const std::string& config_file, std::string& message);
		bool Reinitialize();
		bool UpdateQueue(const std::vector<_target>&, double in_time);	//Add or update target in the priority queue
		bool GetQueue(std::vector<_queue_element>&);
		bool PopQueue(_queue_element&);
		bool ModifyQueue(const _queue_element&, int);
		void SetMode(std::string);
};

