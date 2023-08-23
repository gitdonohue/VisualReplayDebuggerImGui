#pragma once
#include <iosfwd>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>

namespace VisualReplayDebugger
{
	enum Color { AliceBlue, PaleGoldenrod, Orchid, OrangeRed, Orange, OliveDrab, Olive, OldLace, Navy, NavajoWhite, Moccasin, MistyRose, MintCream, MidnightBlue, MediumVioletRed, MediumTurquoise, MediumSpringGreen, MediumSlateBlue, LightSkyBlue, LightSlateGray, LightSteelBlue, LightYellow, Lime, LimeGreen, PaleGreen, Linen, Maroon, MediumAquamarine, MediumBlue, MediumOrchid, MediumPurple, MediumSeaGreen, Magenta, PaleTurquoise, PaleVioletRed, PapayaWhip, SlateGray, Snow, SpringGreen, SteelBlue, Tan, Teal, SlateBlue, Thistle, Transparent, Turquoise, Violet, Wheat, White, WhiteSmoke, Tomato, LightSeaGreen, SkyBlue, Sienna, PeachPuff, Peru, Pink, Plum, PowderBlue, Purple, Silver, Red, RoyalBlue, SaddleBrown, Salmon, SandyBrown, SeaGreen, SeaShell, RosyBrown, Yellow, LightSalmon, LightGreen, DarkRed, DarkOrchid, DarkOrange, DarkOliveGreen, DarkMagenta, DarkKhaki, DarkGreen, DarkGray, DarkGoldenrod, DarkCyan, DarkBlue, Cyan, Crimson, Cornsilk, CornflowerBlue, Coral, Chocolate, AntiqueWhite, Aqua, Aquamarine, Azure, Beige, Bisque, DarkSalmon, Black, Blue, BlueViolet, Brown, BurlyWood, CadetBlue, Chartreuse, BlanchedAlmond, DarkSeaGreen, DarkSlateBlue, DarkSlateGray, HotPink, IndianRed, Indigo, Ivory, Khaki, Lavender, Honeydew, LavenderBlush, LemonChiffon, LightBlue, LightCoral, LightCyan, LightGoldenrodYellow, LightGray, LawnGreen, LightPink, GreenYellow, Gray, DarkTurquoise, DarkViolet, DeepPink, DeepSkyBlue, DimGray, DodgerBlue, Green, Firebrick, ForestGreen, Fuchsia, Gainsboro, GhostWhite, Gold, Goldenrod, FloralWhite, YellowGreen };

	struct Entity; // fwd decl
	struct Point { float x=0, y=0, z=0; };
	struct Quaternion { float x = 0, y = 0, z = 0, w = 1; };
	struct Transform { Point translation; Quaternion rotation; };

	struct FrameRange
	{
		int start = 0;
		int end = -1;

		bool operator==(const FrameRange& other) const { return start == other.start && end == other.end; }
	};
	
	struct TimeRange { float start = 0, end = -1; };

	struct LogEntry
	{
		int id = 0;
		Entity* entity = nullptr;
		int frame = -1;
		float time = 0;
		std::string category;
		std::string message;
		Color color;
	};

	struct Entity
	{
		int Id = -1;
		std::string Name;
		std::string Path;
		std::string TypeName;
		std::string CategoryName;
		Transform InitialTransform;
		std::map<std::string, std::string> StaticParameters;
		int CreationFrame = -1;
		int DestructionFrame = -1;

		int RegistrationFrame = -1;
		bool HasTransforms = false;
		bool HasLogs = false;
		bool HasLogsPastFirstFrame = false;
		bool HasDraws = false;
		bool HasMesh = false;
		bool HasParameters = false;
		bool HasNumericParameters = false;

		std::vector<LogEntry*> Logs;
	};

	class ReplayData
	{

	public:
		void Read(std::istream& input);

		inline const std::map<int, Entity> GetEntities() const { return entities; }

		inline int LogCount() const { return (int)logs.size(); }
		inline const LogEntry& GetLogEntry(int index) const { return logs.at(index); }
		inline const std::vector<LogEntry>& GetLogs() const { return logs; }
		
		inline int FrameCount() const { return (int)frametimes.size() + 1; }
		float GetTotalTime() const;
		float GetTimeForFrame(int frame) const;

	private:
		void ReadInternal(std::istream& input);

	private:
		std::vector<float> frametimes;
		std::list<int> framesForTimes;
		std::map<int, FrameRange> entityLifetimes;

		std::map<int, Entity> entities;
		std::set<std::string> entityCategories;

		std::vector<LogEntry> logs;

	public:
		static int GetColorValue(const Color& c);
	};
}
