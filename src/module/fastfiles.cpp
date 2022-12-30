#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include "console.hpp"

#include <utils/hook.hpp>

static __declspec(naked) void db_load_stub_client(game::native::XZoneInfo*, unsigned int, int)
{
	__asm
	{
		sub esp, 0Ch
		mov eax, [esp + 18h]

		mov ecx, game::native::DB_LoadXAssets
		add ecx, 7h
		push ecx
		ret
	}
}

class fastfiles final : public module
{
public:
	void post_load() override
	{
		utils::hook(game::native::DB_LoadXAssets, db_load_stub, HOOK_JUMP).install()->quick();
	}

private:
	static void db_load_stub(game::native::XZoneInfo* zone_info, const unsigned int zone_count, const int sync)
	{
		for (unsigned int i = 0; i < zone_count; ++i)
		{
			if (zone_info[i].name)
			{
				console::info("Loading FastFile: %s (0x%X | 0x%X)\n", zone_info[i].name, zone_info[i].allocFlags, zone_info[i].freeFlags);
			}
		}

		return db_load_stub_client(zone_info, zone_count, sync);
	}
};

REGISTER_MODULE(fastfiles)
