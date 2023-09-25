#include "../include/InputInterface.h"

bool InputBlock::Initialize(const std::string& xi_config_file, std::string& xo_message)
{
	return true;
}
bool InputBlock::Reinitialize()
{
	Targets.clear();
	TargetsPrev.clear();
	Assets.clear();
	Assets.clear();

	return true;
}

bool InputBlock::AddTargets(const std::vector<_target>& NewTargets)
{
	bool found = false;

	TargetsPrev = Targets;
	for (const auto& i : NewTargets)
	{
		for (int j = 0; j < Targets.size(); j++)
		{
			if (i.ID == Targets[j].ID)
			{
				found = true;
				Targets[j] = i;
				continue;
			}
		}
		if (found == false)
		{
			Targets.push_back(i);
		}
		found = false;
	}

	return true;
}
bool InputBlock::UpdateAssets(const std::vector<_asset>& NewAssets, const double time)
{
	bool found = false;

	t_prev = t;
	t = time;
	AssetsPrev = Assets;
	for (const auto& i : NewAssets)
	{
		for (int j = 0; j < Assets.size(); j++)
		{
			if (i.ID == Assets[j].ID)
			{
				found = true;
				Assets[j] = i;
				continue;
			}
		}
		if (found == false)
		{
			Assets.push_back(i);
		}
		found = false;
	}

	return true;
}

bool InputBlock::GetTargets(std::vector<_target>& xo_Targets, std::vector<_target>& xo_TargetsPrev)
{
	xo_Targets = Targets;
	xo_TargetsPrev = TargetsPrev;

	return true;
}
bool InputBlock::GetAssets(std::vector<_asset>& xo_Assets, std::vector<_asset>& xo_AssetsPrev)
{
	xo_Assets = Assets;
	xo_AssetsPrev = AssetsPrev;

	return true;
}
bool InputBlock::GetInstants(double& xo_t, double& xo_t_prev)
{
	xo_t = t;
	xo_t_prev = t_prev;

	return true;
}