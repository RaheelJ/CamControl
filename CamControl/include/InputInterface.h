#pragma once

#include "../include/Auxiliary.h"

class InputBlock
{
	std::vector<_target> Targets;
	std::vector<_target> TargetsPrev;
	std::vector<_asset> Assets, AssetsPrev;
	double t{ 0 }, t_prev{ 0 };

	public:
		InputBlock() {};
		~InputBlock() {};

		bool Initialize(const std::string& xi_config_file, std::string& xo_message);
		bool Reinitialize();
		bool AddTargets(const std::vector<_target>&, double in_time);
		bool UpdateAssets(const std::vector<_asset>&, double in_time);
		bool GetTargets(std::vector<_target>&, std::vector<_target>&);
		bool GetAssets(std::vector<_asset>&, std::vector<_asset>&);
		bool GetInstants(double&, double&);
};



