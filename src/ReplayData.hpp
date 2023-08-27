#pragma once
#include <iosfwd>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

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

	template <typename T> class TimeStampedList 
		: public std::list<std::pair<int, T>>
	{
		T defaultVal = {};
	public:
		inline const T& AtFrame(int frame) const 
		{
			if (this->size() < 1) return defaultVal;
			const T* last_value = &this->front().second;
			for (const auto& p : *this)
			{
				if (p.first > frame) break;
				last_value = &p.second;
			}
			return *last_value;
		}
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

		std::vector<LogEntry*> Logs;
		TimeStampedList<Transform> Transforms;

		std::map<std::string, TimeStampedList<std::string>> DynamicProperties;
		std::map<std::string, TimeStampedList<float>> DynamicValues;

	protected:
		inline void AddDynamicProperty(const std::string& label, const std::string& value, int frame) 
		{
			DynamicProperties[label].push_back(std::make_pair(frame,value));
		}

		inline void AddDynamicValue(const std::string& label, const float& value, int frame)
		{
			DynamicValues[label].push_back(std::make_pair(frame, value));
		}

		friend class ReplayData;
	};

	
	enum EntityDrawCommandType { None, Line, Circle, Sphere, Box, Capsule, Mesh };

	struct EntityDrawCommand
	{
		const Entity* entity = nullptr;
		int frame = -1;
		std::string category;
		EntityDrawCommandType type;
		Color color;
		Transform xform;
		Point p2;
		float scale = 1;
		std::vector<Point> verts;
	};

	class ReplayData
	{

	public:
		void Read(std::istream& input);

		inline const std::list<const Entity*> GetEntities() const { return entities_refs; }
		inline const Entity* GetEntity(int id) const { return entities_map.at(id); }

		inline int LogCount() const { return (int)logs.size(); }
		inline const LogEntry& GetLogEntry(int index) const { return logs.at(index); }
		inline const std::vector<LogEntry>& GetLogs() const { return logs; }
		
		inline int FrameCount() const { return (int)frametimes.size() + 1; }
		float GetTotalTime() const;
		float GetTimeForFrame(int frame) const;

	private:
		void ReadInternal(std::istream& input);
		inline Entity& GetEntityRef(int id) { return *(const_cast<Entity*>(entities_map.at(id))); }

	private:
		std::vector<float> frametimes;
		std::list<int> framesForTimes;
		std::map<int, FrameRange> entityLifetimes;

		std::list<Entity> entities;
		std::list<const Entity*> entities_refs;
		std::map<int, const Entity*> entities_map;
		std::set<std::string> entityCategories;

		std::vector<LogEntry> logs;
		std::vector<EntityDrawCommand> drawCommands;

	public:
		static int GetColorValue(const Color& c);
	};
}
