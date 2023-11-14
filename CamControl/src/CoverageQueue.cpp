#include "../include/CoverageQueue.h"
#include "../libs/pugixml/pugixml.hpp"
#include <iostream>

bool CoverageBlock::Initialize(const std::string& config_file, std::string& message)
{
	pugi::xml_document Doc;
	int error = 0;
	try
	{
		pugi::xml_parse_result Result = Doc.load_file(config_file.c_str());		/* Read "config_file" */
		if (Result) {}
		else {
			throw "File not found or formatting error!";	                    /* Throw exception if file not read */
		}
	}
	catch (const char* error_2)
	{
		message = "File not found or formatting error!";
		std::cerr << error_2 << "\n";							/* Print the error message */
		return false;
	}
	pugi::xml_node Root = Doc.child("CamControl");				/* Accessing Root Node */
	pugi::xml_node CoverageNode = Root.child("CoverageQueue");
	pugi::xml_node WeightsNode = CoverageNode.child("Weights");
	if (Root && WeightsNode && CoverageNode) {}
	else {
		message = "Root, CoverageQueue or Weights node not found!";
		return false;
	}

	/* Initialize parameters from "Weights" node */
	Weights.W_threat = ReadParam(std::stod(WeightsNode.child_value("W_threat")), 0, 1, error);
	Weights.W_std_deg = ReadParam(std::stod(WeightsNode.child_value("W_std_deg")), 0, 1, error);
	Weights.W_std = ReadParam(std::stod(WeightsNode.child_value("W_std")), 0, 1, error);
	Weights.W_type = ReadParam(std::stod(WeightsNode.child_value("W_type")), 0, 1, error);
	Weights.W_detect = ReadParam(std::stod(WeightsNode.child_value("W_detect")), 0, 1, error);

	region_priority_method = CoverageNode.child_value("region_priority_method");
	if (region_priority_method == "user_defined")
	{
		region_share = ReadParam(std::stod(CoverageNode.child_value("region_share")), 0, 100, error);
		region_share = region_share / 100;
	}
	/* Initialize parameters from "RegionOfInterest" node */
	_region_interest temp_region{ 0 };
	pugi::xml_node temp_node, point;
	for (point = CoverageNode.child("RegionOfInterest"); point; point = point.next_sibling("RegionOfInterest"))
	{
		if (region_priority_method == "user_defined")
		{
			temp_region.priority = ReadParam(std::stod(point.child_value("region_priority")), 0, 1, error);
		}
		temp_region.ID = ReadParam(std::stoi(point.child_value("id")), -9999, -1, error);
		temp_node = point.child("Point");
		temp_region.pointA.alt = ReadParam(temp_node.attribute("Altitude").as_double(), 0, 8e6, error);
		temp_region.pointA.lat = ReadParam(temp_node.attribute("Latitude").as_double(), -90, 90, error);
		temp_region.pointA.lon = ReadParam(temp_node.attribute("Longitude").as_double(), -180, 180, error);
		RegionsOfInterest.push_back(temp_region);
	}

	/* Check for out of range parameters */
	if (error == 1) {
		message = std::to_string(error) + " parameter is out of range";
		return false;
	}
	else if (error > 1) {
		message = std::to_string(error) + " parameters are out of range";
		return false;
	}

	PriorityQueue.clear();
	UpdateQueue();
	return true;
}
bool CoverageBlock::Reinitialize()
{
	PriorityQueue.clear();
	return true;
}

void CoverageBlock::SetMode(std::string in_Mode)
{
	if (in_Mode == "SCAN")
	{
		operation_mode = 0;
	}
	else 
	{
		//Tracking Mode
		operation_mode = 1;
	}
}
bool CoverageBlock::NormalizeQueue()
{
	double total_priority{ 0 };
	for (const auto& i : PriorityQueue)
	{
		total_priority += i.Npriority;
	}
	
	if (total_priority > 0)
	{
		for (int j = 0; j < PriorityQueue.size(); j++)
		{
			PriorityQueue[j].priority = PriorityQueue[j].Npriority / total_priority;
		}
	}

	return true;
}
bool CoverageBlock::NormalizeTargets(const double sum_priority)
{
	double total_priority{ 0 };
	for (const auto& i : PriorityQueue)
	{
		if (i.ID >= 0)
		{
			total_priority += i.Npriority;
		}
	}

	if (total_priority > 0)
	{
		for (int j = 0; j < PriorityQueue.size(); j++)
		{
			if (PriorityQueue[j].ID >= 0)
			{
				PriorityQueue[j].priority = sum_priority * PriorityQueue[j].Npriority / total_priority;
			}
		}
	}

	return true;
}
bool CoverageBlock::NormalizeRegions(const double sum_priority)
{
	double total_priority{ 0 };
	for (const auto& i : PriorityQueue)
	{
		if (i.ID <  0)
		{
			total_priority += i.Npriority;
		}
	}

	if (total_priority > 0)
	{
		for (int j = 0; j < PriorityQueue.size(); j++)
		{
			if (PriorityQueue[j].ID < 0)
			{
				PriorityQueue[j].priority = sum_priority * PriorityQueue[j].Npriority / total_priority;
			}
		}
	}

	return true;
}

bool CoverageBlock::GetQueue(std::vector<_queue_element>& xo_PriorityQueue)
{
	if (operation_mode = 0)
	{
		xo_PriorityQueue.clear();
		return true;
	}
	xo_PriorityQueue = PriorityQueue;
	return true;
}
bool CoverageBlock::PopQueue(_queue_element& QElement)
{
	if (operation_mode == 0)
	{
		QElement.ID = -99;			//ID for camera scan
	}
	else if (current_time >= scan_time)
	{
		QElement.ID = -99;			//ID for camera scan
	}

	double temp_priority{ 0 };
	int pop_index{ -1 };
	double rand_num = RandNum(0.0, 1.0);
	for (int i = 0; i < PriorityQueue.size(); i++)
	{
		temp_priority = PriorityQueue[i].priority + temp_priority;
		if (rand_num <= temp_priority)
		{
			pop_index = i;
		}
	}
	if (pop_index >= 0)
	{
		QElement = PriorityQueue[pop_index];
		PriorityQueue[pop_index].count++;
	}
	
	return true;
}

bool CoverageBlock::UpdateQueue(const std::vector<_target>& Targets, double in_time)
{
	if (in_time > current_time)
	{
		current_time = in_time;
	}
	if (operation_mode == 0)
	{
		return true;
	}

	bool found = false;
	_queue_element temp_element{ 0 };

	for (const auto& i:Targets)
	{
		for (int j=0; j<PriorityQueue.size(); j++)
		{
			if (i.ID == PriorityQueue[j].ID)
			{
				found = true;
				PriorityQueue[j].Npriority = CalcPriority(i);
				continue;
			}
		}
		if (found == false)
		{
			temp_element.count = 0;
			temp_element.ID = i.ID;
			temp_element.last_stoppage_criteria = 0;
			temp_element.last_coverage_time = -1;
			temp_element.Npriority = CalcPriority(i);
			PriorityQueue.push_back(temp_element);
		}
		found = false;
	}

	if (region_priority_method == "average")
	{
		UpdateQueue();
	}
	else
	{
		NormalizeTargets(1 - region_share);
	}

	return true;
}
double CoverageBlock::CalcPriority(const _target& Target)
{
	double priority = 0;
	double factor_threat{ 0 }, factor_detection_confidence{ 0 }, factor_std{ 0 }, factor_type{ 0 };

	factor_threat = Weights.W_threat * Target.threat.level;
	factor_std = Weights.W_std_deg * (sqrt(pow(Target.pos.std_lat_y, 2) + pow(Target.pos.std_lon_x, 2))) + Weights.W_std * (abs(Target.pos.std_alt_z));
	factor_detection_confidence = Weights.W_detect * Target.threat.detect_confidence;
	factor_type = Weights.W_type * Target.type.size;

	priority = factor_threat + factor_std - factor_detection_confidence + factor_type;
	return priority;
}

bool CoverageBlock::UpdateQueue()
{
	bool found = false;
	_queue_element temp_element{ 0 };

	for (const auto& i : RegionsOfInterest)
	{
		for (int j = 0; j < PriorityQueue.size(); j++)
		{
			if (i.ID == PriorityQueue[j].ID)
			{
				found = true;
				PriorityQueue[j].Npriority = CalcPriority(i);
				continue;
			}
		}
		if (found == false)
		{
			temp_element.ID = i.ID;
			temp_element.count = 0;
			temp_element.last_coverage_time = -1;
			temp_element.last_stoppage_criteria = 0;
			temp_element.Npriority = CalcPriority(i);
			PriorityQueue.push_back(temp_element);
		}
		found = false;
	}

	if (region_priority_method == "average")
	{
		NormalizeQueue();
	}
	else
	{
		NormalizeRegions(region_share);
	}
	return true;
}
double CoverageBlock::CalcPriority(const _region_interest& Region)
{
	double priority_avg{ 0 };
	int total_elements{ 0 };

	if (region_priority_method == "average")
	{
		for (const auto& i : PriorityQueue)
		{
			if (i.ID < 0)
			{
				continue;		//-ve ID represents regions of interest and does not add to the priority of other regions
			}
			priority_avg += i.priority;
			total_elements++;
		}
		priority_avg = (total_elements <= 0) ? 1 : priority_avg / total_elements;		//set priority as 1 if the queue is empty
	}
	else
	{
		priority_avg = Region.priority;
	}
	return priority_avg;
}

bool CoverageBlock::ModifyQueue(const _queue_element& element, int operation)
{
	auto ref = PriorityQueue.begin();
	int index = 0;
	for (const auto& i: PriorityQueue)
	{
		if (i.ID == element.ID)
		{
			break;
		}
		ref++;
		index++;
	}

	switch (operation)
	{
	case -1:
		PriorityQueue.erase(ref);
		if (region_priority_method == "average")
		{
			UpdateQueue();
		}
		else
		{
			NormalizeTargets(1 - region_share);
		}
		break;
	case 0:
		PriorityQueue[index].last_stoppage_criteria = 0;
		break;
	case 1:
		PriorityQueue[index].last_stoppage_criteria = 1;
		break;
	case 2:
		PriorityQueue[index].last_stoppage_criteria = 2;
		break;
	default:
		break;
	}

	return true;
}