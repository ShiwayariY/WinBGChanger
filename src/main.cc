/* __________ define in CMakeLists __________ */
// WINVER=_WIN32_WINNT_WIN10
// _WIN32_WINNT=_WIN32_WINNT_WIN10

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include <windows.h>
#include <shobjidl.h>
#include <comdef.h>

#include <SubcommandPicker.hh>

_COM_SMARTPTR_TYPEDEF(IDesktopWallpaper, __uuidof(IDesktopWallpaper));

struct ComEnable {
	ComEnable() {
		if (FAILED(CoInitialize(nullptr)))
			throw std::runtime_error{ "CoInitialize failed" };
	}
	~ComEnable() noexcept { CoUninitialize(); }
};

std::wstring stows(const std::string& s) {
	int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
	std::wstring ws(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &ws[0], size);
	return ws;
}

[[nodiscard]] std::wstring get_monitor_id(IDesktopWallpaper* pWallpaper, uint32_t index) {
	wchar_t* id_wstr = nullptr;
	if (FAILED(pWallpaper->GetMonitorDevicePathAt(index, &id_wstr)))
		throw std::runtime_error{ "Failed to get monitor id" };

	if (!id_wstr) return {};

	auto free_buf = [](void* p) noexcept { CoTaskMemFree(p); };
	std::unique_ptr<wchar_t, decltype(free_buf)> safe_buf{ id_wstr, free_buf };
	return id_wstr;
}

void set_wallpaper(int argc, char** argv, IDesktopWallpaper* pWallpaper) {
	uint32_t monitor_count{};
	if (FAILED(pWallpaper->GetMonitorDevicePathCount(&monitor_count)))
		throw std::runtime_error{ "Failed to get monitor count" };

	std::vector<std::wstring> wallpaper_paths;
	for (int i = 1; i < argc; ++i)
		wallpaper_paths.push_back(stows(argv[i]));
	wallpaper_paths.resize(monitor_count);

	for (uint32_t i = 0; i < monitor_count; ++i) {
		const auto id = get_monitor_id(pWallpaper, i);
		if (SUCCEEDED(pWallpaper->SetWallpaper(id.c_str(), wallpaper_paths[i].c_str())))
			std::wcout << "Set";
		else
			std::wcout << "Failed to set";
		std::wcout << " monitor " << i << " wallpaper to " << wallpaper_paths[i] << "\n";
	}
}

enum class Subcommand {
	Enable,
	Disable,
	Wallpaper
};

int main(int argc, char** argv) {
	ComEnable com;

	IDesktopWallpaperPtr pWallpaper{};
	if (FAILED(pWallpaper.CreateInstance(CLSID_DesktopWallpaper)))
		throw std::runtime_error{ "Failed to create IDesktopWallpaper instance" };

	shimiyuu::SubcommandPicker<Subcommand> subcmd_picker{
		{ { "on", Subcommand::Enable },
		  { "off", Subcommand::Disable },
		  { "wallpaper", Subcommand::Wallpaper } }
	};
	subcmd_picker.set_default(Subcommand::Wallpaper);
	switch (subcmd_picker.pick(argc, argv)) {
		case Subcommand::Enable:
			pWallpaper->Enable(TRUE);
			break;
		case Subcommand::Disable:
			pWallpaper->Enable(FALSE);
			break;
		case Subcommand::Wallpaper:
			set_wallpaper(argc, argv, pWallpaper);
			break;
	}

	return 0;
}