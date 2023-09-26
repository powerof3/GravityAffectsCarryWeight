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

				// higher grav = less carry weight, lower grav = more carry weight
				carryWeight *= Settings::GetSingleton()->baseGravity / localGravity;
			}
			RE::BGSEntryPoint::HandleEntryPoint(RE::BGSEntryPoint::ENTRY_POINT::kGetMaxCarryWeight, a_actor, std::addressof(carryWeight));
			return carryWeight;
		}
		static inline constexpr std::size_t size{ 0xBA };
	};

	struct AdjustItemWeight
	{
		static float thunk(RE::Actor* a_this, std::uintptr_t a_inventoryStack, float a_itemWeight)
		{
			if (a_itemWeight > 0.0f) {
				auto localGravity = a_this->GetGravityScale();
				if (localGravity < 0.1f) {
					localGravity = 0.1f;
				}

				// higher grav = weigh more, lower grav = weight less
			    float itemWeight = a_itemWeight * localGravity / Settings::GetSingleton()->baseGravity;

			    return func(a_this, a_inventoryStack, itemWeight);
			}

			return func(a_this, a_inventoryStack, a_itemWeight);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::size_t                      idx = 0x99;
	};

	void Install()
	{
		const auto settings = Settings::GetSingleton();
		settings->LoadSettings();

		if (settings->adjustItemWeights) {
			stl::write_vfunc<AdjustItemWeight>(RE::VTABLE::PlayerCharacter[38]);
		} else {
			REL::Relocation<std::uintptr_t> target{ REL::ID(150799) };
			stl::asm_replace<GetMaxCarryWeight>(target.address());
		}
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
	data.UsesAddressLibrary(true);
	//data.UsesSigScanning(true);
	data.IsLayoutDependent(true);
	//data.HasNoStructUse(true);
	data.CompatibleVersions({ SFSE::RUNTIME_LATEST });

	return data;
}();

DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)
{
	SFSE::Init(a_sfse);

	SFSE::GetMessagingInterface()->RegisterListener(MessageCallback);

	return true;
}
