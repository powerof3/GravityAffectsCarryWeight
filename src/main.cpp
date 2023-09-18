#include "Settings.h"

namespace Handler
{
	struct GetMaxCarryWeight
	{
		static float func(RE::Actor* a_actor, bool a_useGravity)
		{
			auto carryWeight = a_actor->GetActorValue(*RE::ActorValue::GetSingleton()->carryWeight);
			if (a_useGravity) {
                auto localGravity = a_actor->GetGravityScale();
				if (localGravity < 0.1f) {
					localGravity = 0.1f;
				}

				carryWeight *= Settings::GetSingleton()->baseGravity / localGravity;
			}
			RE::BGSEntryPoint::HandleEntryPoint(RE::BGSEntryPoint::ENTRY_POINT::kGetMaxCarryWeight, a_actor, std::addressof(carryWeight));
			return carryWeight;
		}
		static inline constexpr std::size_t size{ 0xBA };
	};

	void Install()
	{
		Settings::GetSingleton()->LoadSettings();

	    REL::Relocation<std::uintptr_t> target{ REL::Offset(0x024877CC) };
		stl::asm_replace<GetMaxCarryWeight>(target.address());
	}
}

void MessageCallback(SFSE::MessagingInterface::Message* a_msg) noexcept
{
	switch (a_msg->type) {
	case SFSE::MessagingInterface::kPostLoad:
		Handler::Install();
		break;
	default:
		break;
	}
}

DLLEXPORT constinit auto SFSEPlugin_Version = []() noexcept {
	SFSE::PluginVersionData data{};

	data.PluginVersion(Version::MAJOR);
	data.PluginName(Version::PROJECT);
	data.AuthorName("powerofthree");
	data.UsesSigScanning(true);
	//data.UsesAddressLibrary(true);
	data.HasNoStructUse(true);
	//data.IsLayoutDependent(true);
	data.CompatibleVersions({ SFSE::RUNTIME_SF_1_7_29 });

	return data;
}();

DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)
{
	SFSE::Init(a_sfse);

	SFSE::GetMessagingInterface()->RegisterListener(MessageCallback);

	return true;
}
