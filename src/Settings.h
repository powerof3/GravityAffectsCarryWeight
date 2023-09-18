#pragma once

class Settings : public ISingleton<Settings>
{
public:
	void LoadSettings()
	{
        const auto path = std::format("Data/SFSE/Plugins/{}.ini", Version::PROJECT);

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path.c_str());

		ini::get_value(ini, baseGravity, "Settings", "fGravityBase", ";Base gravity scale.");
		ini::get_value(ini, adjustItemWeights, "Settings", "bAdjustItemWeights", ";Gravity affects item weight. Items will weigh less in lower gravity.\n;If false, gravity will affect carry weight. You will have more carry weight in lower gravity");

		(void)ini.SaveFile(path.c_str());
	}

	// members
	float baseGravity{ 1.0 };
	bool  adjustItemWeights{ true };
};
