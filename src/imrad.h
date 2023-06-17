#pragma once
#include <string>
#include <vector>
#include <functional>
#include <imgui.h>
#include <fstream> //Save/LoadStyle
#include <iomanip>
#include <sstream> 
#include <map>
#include <imgui_internal.h> //for CurrentItemFlags
#include <misc/cpp/imgui_stdlib.h> //for Input(std::string)

#ifdef IMRAD_WITH_FMT
#include <fmt/format.h>
#endif

#ifdef IMRAD_WITH_GLFW_TEXTURE
#include <stb_image.h> //for LoadTextureFromFile
#include <GLFW/glfw3.h> //for LoadTextureFromFile
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#endif

namespace ImRad {

using Int2 = int[2];
using Int3 = int[3];
using Int4 = int[4];
using Float2 = float[2];
using Float3 = float[3];
using Float4 = float[4];
using Color3 = ImVec4;
using Color4 = ImVec4;

enum ModalResult {
	None = 0,
	Ok = 0x1,
	Cancel = 0x2,
	Yes = 0x4,
	No = 0x8,
	Abort = 0x10,
	Retry = 0x20,
	Ignore = 0x40,
	All = 0x80,
};

enum Alignment {
	AlignLeft = 0x1,
	AlignRight = 0x2,
	AlignHCenter = 0x4,
	AlignTop = 0x8,
	AlignBottom = 0x10,
	AlignVCenter = 0x20,
};

struct Texture
{
	ImTextureID id = 0;
	int w, h;
	explicit operator bool() const { return id != 0; }
};

struct CustomWidgetArgs
{
	ImVec2 size;

	CustomWidgetArgs(const ImVec2& sz) : size(sz) {}
	CustomWidgetArgs(float x, float y) : size(x, y) {}
};

inline bool Combo(const char* label, int* curr, const std::vector<std::string>& items, int maxh = -1)
{
	//todo: BeginCombo/Selectable
	std::vector<const char*> citems(items.size());
	for (size_t i = 0; i < items.size(); ++i)
		citems[i] = items[i].c_str();
	return ImGui::Combo(label, curr, citems.data(), (int)citems.size(), maxh);
}

inline bool IsItemDoubleClicked()
{
	return ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered();
}

inline bool IsItemDisabled()
{
	return ImGui::GetCurrentContext()->CurrentItemFlags & ImGuiItemFlags_Disabled;
}

inline void Spacing(int n)
{
	while (n--)
		ImGui::Spacing();
}

inline bool TableNextColumn(int n)
{
	bool b;
	while (n--)
		b = ImGui::TableNextColumn();
	return b;
}

inline void NextColumn(int n)
{
	while (n--)
		ImGui::NextColumn();
}

inline std::string Format(std::string_view fmt)
{
	return std::string(fmt);
}

template <class A1, class... A>
std::string Format(std::string_view fmt, A1&& arg, A&&... args)
{
#ifdef IMRAD_WITH_FMT
	return fmt::format(fmt, std::forward<A1>(arg), std::forward<A>(args)...);
#else
	//todo
	std::string s;
	for (size_t i = 0; i < fmt.size(); ++i)
	{
		if (fmt[i] == '{') {
			if (i + 1 == fmt.size())
				break;
			if (fmt[i + 1] == '{')
				s += '{';
			else {
				auto j = fmt.find('}', i + 1);
				if (j == std::string::npos)
					break;
				if constexpr (std::is_same_v<std::decay_t<A1>, std::string>)
					s += arg;
				else
					s += std::to_string(arg);
				return s + Format(fmt.substr(j + 1), args...);
			}
		}
		else
			s += fmt[i];
	}
	return s;
#endif
}


#ifdef IMRAD_WITH_GLFW_TEXTURE
// Simple helper function to load an image into a OpenGL texture with common settings
// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
inline Texture LoadTextureFromFile(std::string_view filename)
{
	// Load from file
	Texture tex;
	std::string tmp(filename);
	unsigned char* image_data = stbi_load(tmp.c_str(), &tex.w, &tex.h, NULL, 4);
	if (image_data == NULL)
		return {};

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	tex.id = (ImTextureID)(intptr_t)image_texture;
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.w, tex.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	return tex;
}
#else
Texture LoadTextureFromFile(const std::string& filename);
#endif

//For debugging pruposes
inline void SaveStyle(std::string_view fname)
{
	std::ofstream fout(std::string(fname.begin(), fname.end()));
	const auto& s = ImGui::GetStyle();

	fout << "[colors]\n";
	for (int i = 0; i < ImGuiCol_COUNT; ++i) {
		fout << ImGui::GetStyleColorName(i) << " = ";
		const auto& clr = s.Colors[i];
		fout << int(clr.x * 255) << " " << int(clr.y * 255) << " " 
			<< int(clr.z * 255) << " " << int(clr.w * 255) << "\n";
	}

	fout << "\n[variables]\n";
#define WRITE_FLT(a) fout << #a " = " << s.a << "\n"
#define WRITE_VEC(a) fout << #a " = " << s.a.x << " " << s.a.y << "\n"
	
	WRITE_FLT(Alpha);
	WRITE_FLT(DisabledAlpha);
	WRITE_VEC(WindowPadding);
	WRITE_FLT(WindowRounding);
	WRITE_FLT(WindowBorderSize);
	WRITE_VEC(WindowMinSize);
	WRITE_VEC(WindowTitleAlign);
	WRITE_FLT(ChildRounding);
	WRITE_FLT(ChildBorderSize);
	WRITE_FLT(PopupRounding);
	WRITE_FLT(PopupBorderSize);
	WRITE_VEC(FramePadding);
	WRITE_FLT(FrameRounding);
	WRITE_FLT(FrameBorderSize);
	WRITE_VEC(ItemSpacing);
	WRITE_VEC(ItemInnerSpacing);
	WRITE_VEC(CellPadding);
	WRITE_FLT(IndentSpacing);
	WRITE_FLT(ScrollbarSize);
	WRITE_FLT(ScrollbarRounding);
	WRITE_FLT(TabRounding);
	WRITE_FLT(TabBorderSize);
#undef WRITE_FLT
#undef WRITE_VEC

	fout << "\n[fonts]\n";
	fout << "# Default = \"Roboto-Medium.ttf\" size 20\n";
	
	fout << "\n[imrad.colors]\n";
	fout << "# Hovered 255 255 0 255\n";
	fout << "# Selected 255 0 0 255\n";
	fout << "# Snap1 ...\n";
}

//This function can be used in your code to load style and fonts from the INI file
//It is also used by ImRAD when switching themes
inline void LoadStyle(std::string_view fname, float fontScaling = 1, ImGuiStyle* dst = nullptr, std::map<std::string, ImFont*>* fontMap = nullptr, std::map<std::string, std::string>* extra = nullptr)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	*style = ImGuiStyle();
	auto& io = ImGui::GetIO();

	std::string parentPath(fname);
	size_t ix = parentPath.find_last_of("/\\");
	if (ix != std::string::npos)
		parentPath.resize(ix + 1);

	std::ifstream fin(std::string(fname.begin(), fname.end()));
	if (!fin)
		throw std::runtime_error("Can't read " + std::string(fname));
	std::string line;
	std::string cat;
	int lastClr = -1;
	std::string lastFont;
	while (std::getline(fin, line)) 
	{
		if (line.empty() || line[0] == ';' || line[0] == '#')
			continue;
		else if (line[0] == '[' && line.back() == ']')
			cat = line.substr(1, line.size() - 2);
		else
		{
			size_t i1 = line.find_first_not_of("=\t ", 0);
			size_t i2 = line.find_first_of("=\t ", i1);
			if (i1 == std::string::npos || i2 == std::string::npos)
				continue;
			std::string key = line.substr(i1, i2 - i1);
			i1 = line.find_first_not_of("=\t ", i2);
			std::istringstream is(line.substr(i1));
			
			if (cat == "colors")
			{
				for (int i = lastClr + 1; i != lastClr; i = (i + 1) % ImGuiCol_COUNT)
					if (key == ImGui::GetStyleColorName(i)) {
						lastClr = i;
						int r, g, b, a;
						is >> r >> g >> b >> a;
						style->Colors[i].x = r / 255.f;
						style->Colors[i].y = g / 255.f;
						style->Colors[i].z = b / 255.f;
						style->Colors[i].w = a / 255.f;
						break;
					}
			}
			else if (cat == "variables")
			{
#define READ_FLT(a) if (key == #a) is >> style->a;
#define READ_VEC(a) if (key == #a) is >> style->a.x >> style->a.y;
				
				READ_FLT(Alpha);
				READ_FLT(DisabledAlpha);
				READ_VEC(WindowPadding);
				READ_FLT(WindowRounding);
				READ_FLT(WindowBorderSize);
				READ_VEC(WindowMinSize);
				READ_VEC(WindowTitleAlign);
				READ_FLT(ChildRounding);
				READ_FLT(ChildBorderSize);
				READ_FLT(PopupRounding);
				READ_FLT(PopupBorderSize);
				READ_VEC(FramePadding);
				READ_FLT(FrameRounding);
				READ_FLT(FrameBorderSize);
				READ_VEC(ItemSpacing);
				READ_VEC(ItemInnerSpacing);
				READ_VEC(CellPadding);
				READ_FLT(IndentSpacing);
				READ_FLT(ScrollbarSize);
				READ_FLT(ScrollbarRounding);
				READ_FLT(TabRounding);
				READ_FLT(TabBorderSize);
#undef READ_FLT
#undef READ_VEC
			}
			else if (cat == "fonts")
			{
				std::string path;
				float size = 20;
				bool hasRange = false;
				static std::vector<std::unique_ptr<ImWchar[]>> rngs;
				
				is >> std::quoted(path);
				bool isAbsolute = path.size() >= 2 && (path[0] == '/' || path[1] == ':');
				if (!isAbsolute)
					path = parentPath + path;
				std::string tmp;
				while (is >> tmp) 
				{
					if (tmp == "size")
						is >> size;
					else if (tmp == "range") {
						hasRange = true;
						//needs to outlive this function
						rngs.push_back(std::unique_ptr<ImWchar[]>(new ImWchar[3]));
						is >> rngs.back()[0] >> rngs.back()[1];
						rngs.back()[2] = 0;
					}
				}

				ImFontConfig cfg;
				strncpy(cfg.Name, key.c_str(), sizeof(cfg.Name));
				cfg.Name[sizeof(cfg.Name) - 1] = '\0';
				cfg.MergeMode = key == lastFont;
				cfg.GlyphRanges = hasRange ? rngs.back().get() : nullptr;
				ImFont* fnt = io.Fonts->AddFontFromFileTTF(path.c_str(), size * fontScaling, &cfg);
				if (!fnt)
					throw std::runtime_error("Can't load " + path);
				if (!cfg.MergeMode && fontMap)
					(*fontMap)[lastFont == "" ? "" : key] = fnt;
			
				lastFont = key;
			}
			else if (extra)
			{
				(*extra)[cat + "." + key] = is.str();
			}
		}
	}
	if (fontMap && !(*fontMap).count(""))
		(*fontMap)[""] = io.Fonts->AddFontDefault();
}

//This function will be called from the generated code when alternate font is used
inline ImFont* GetFontByName(std::string_view name)
{
	const auto& io = ImGui::GetIO();
	for (const auto& cfg : io.Fonts->ConfigData) {
		if (cfg.MergeMode)
			continue;
		if (name == cfg.Name)
			return cfg.DstFont;
	}
	return nullptr;
}

}