#include <std_include.hpp>

#include "igfximage.hpp"
#include "../asset_dumper.hpp"

#include "utils/io.hpp"
#include "utils/stream.hpp"
#include "utils/string.hpp"

#include "module/console.hpp"
#include "module/command.hpp"

#define IW4X_IMG_VERSION "0"
namespace asset_dumpers
{

	void igfximage::dump(game::native::XAssetType type, game::native::XAssetHeader header)
	{
		assert(type == game::native::XAssetType::ASSET_TYPE_IMAGE);
		dump(header.image);
	}


	void igfximage::dump(game::native::GfxImage* image)
	{
		if (!image) return;
		std::string name = image->name;

		if (image->category != game::native::GfxImageCategory::IMG_CATEGORY_LOAD_FROM_FILE && image->texture.loadDef)
		{
			if (name[0] == '*') name.erase(name.begin());

			utils::stream buffer;
			buffer.saveArray("IW4xImg" IW4X_IMG_VERSION, 8); // just stick version in the magic since we have an extra char

			buffer.saveObject(static_cast<unsigned char>(image->mapType));
			buffer.saveObject(image->semantic);
			buffer.saveObject(image->category);

			buffer.saveObject(image->texture.loadDef->resourceSize);
			buffer.save(image->texture.loadDef, 16 + image->texture.loadDef->resourceSize);

			utils::io::write_file(std::format("{}/images/{}.iw4xImage", asset_dumper::export_path, name), buffer.toBuffer());
		}
		else
		{
			char* buffer = nullptr;
			auto size = game::native::FS_ReadFile(std::format("images/{}.iwi", image->name).data(), &buffer);
			
			if (size <= 0)
			{
				// Ignore that
				if (std::string(image->name).starts_with("watersetup")) {
					return;
				}

				console::info("Image %s not found, mapping to normalmap!\n", name.data());
				size = game::native::FS_ReadFile("images/$identitynormalmap.iwi", &buffer);
			}

			if (size > 0)
			{
				utils::io::write_file(std::format("{}/images/{}.iwi", asset_dumper::export_path, image->name), std::string(buffer, size));
			}
			else
			{
				console::info("Unable to map to normalmap, this should not happen!\n");
			}
		}
	}

	//int igfximage::StoreTexture()
	//{
	//	game::native::GfxImageLoadDef** loadDef = *reinterpret_cast<game::native::GfxImageLoadDef***>(0xE34814);
	//	game::native::GfxImage* image = *reinterpret_cast<game::native::GfxImage**>(0xE346C4);

	//	size_t size = 16 + (*loadDef)->resourceSize;
	//	void* data = Loader::GetAlloctor()->allocate(size);
	//	std::memcpy(data, *loadDef, size);

	//	image->texture.loadDef = reinterpret_cast<game::native::GfxImageLoadDef*>(data);

	//	return 0;
	//}

	//void igfximage::ReleaseTexture(game::native::XAssetHeader header)
	//{
	//	if (header.image && header.image->texture.loadDef)
	//	{
	//		Loader::GetAlloctor()->free(header.image->texture.loadDef);
	//	}
	//}

	igfximage::igfximage()
	{
		command::add("dumpGfxImage", [](const command::params& params)
			{
				if (params.size() < 2) return;

				game::native::GfxImage image;
				image.name = params[1];
				image.texture.loadDef = nullptr;

				igfximage::dump(&image);
			});

	//	Utils::Hook(0x616E80, igfximage::StoreTexture, HOOK_JUMP).install()->quick();
	//	Utils::Hook(0x488C00, igfximage::ReleaseTexture, HOOK_JUMP).install()->quick();
	}

	//igfximage::~IGfxImage()
	//{

	//}

}