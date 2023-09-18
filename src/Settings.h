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

		ini::get_value(ini, baseGravity, "Settings", "fGravityBase", nullptr);

		(void)ini.SaveFile(path.c_str());
	}

	// members
	float baseGravity{ 1.0 };
};
