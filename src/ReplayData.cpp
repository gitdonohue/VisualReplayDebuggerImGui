#include "ReplayData.hpp"

#include "inflatecpp\decompressor.h"
#include <sstream>

#include <assert.h>

#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace VisualReplayDebugger;

// fwd decl
const Entity ReadEntity(std::istream& input);
const std::string ReadString(std::istream& input);
const std::map<std::string, std::string> ReadStringDict(std::istream& input);
int Read7BitEncodedInt(std::istream& input);

enum BlockType
{
	None = 0,
	FrameStep,
	EntityDef,
	EntityUndef,
	EntitySetPos,
	EntitySetTransform,
	EntityLog,
	EntityParameter,
	EntityValue,
	EntityLine,
	EntityCircle,
	EntitySphere,
	EntityCapsule,
	EntityMesh,
	EntityBox,

	ReplayHeader = 0xFF
};

// Colors ABGR values
static const std::vector<unsigned int> ColorValuesABGR
{
	0xFFFFF8F0, // AliceBlue
	0xFFAAE8EE, // PaleGoldenrod
	0xFFD670DA, // Orchid
	0xFF0045FF, // OrangeRed
	0xFF00A5FF, // Orange
	0xFF238E6B, // OliveDrab
	0xFF008080, // Olive
	0xFFE6F5FD, // OldLace
	0xFF800000, // Navy
	0xFFADDEFF, // NavajoWhite
	0xFFB5E4FF, // Moccasin
	0xFFE1E4FF, // MistyRose
	0xFFFAFFF5, // MintCream
	0xFF701919, // MidnightBlue
	0xFF8515C7, // MediumVioletRed
	0xFFCCD148, // MediumTurquoise
	0xFF9AFA00, // MediumSpringGreen
	0xFFEE687B, // MediumSlateBlue
	0xFFFACE87, // LightSkyBlue
	0xFF998877, // LightSlateGray
	0xFFDEC4B0, // LightSteelBlue
	0xFFE0FFFF, // LightYellow
	0xFF00FF00, // Lime
	0xFF32CD32, // LimeGreen
	0xFF98FB98, // PaleGreen
	0xFFE6F0FA, // Linen
	0xFF000080, // Maroon
	0xFFAACD66, // MediumAquamarine
	0xFFCD0000, // MediumBlue
	0xFFD355BA, // MediumOrchid
	0xFFDB7093, // MediumPurple
	0xFF71B33C, // MediumSeaGreen
	0xFFFF00FF, // Magenta
	0xFFEEEEAF, // PaleTurquoise
	0xFF9370DB, // PaleVioletRed
	0xFFD5EFFF, // PapayaWhip
	0xFF908070, // SlateGray
	0xFFFAFAFF, // Snow
	0xFF7FFF00, // SpringGreen
	0xFFB48246, // SteelBlue
	0xFF8CB4D2, // Tan
	0xFF808000, // Teal
	0xFFCD5A6A, // SlateBlue
	0xFFD8BFD8, // Thistle
	0x00FFFFFF, // Transparent
	0xFFD0E040, // Turquoise
	0xFFEE82EE, // Violet
	0xFFB3DEF5, // Wheat
	0xFFFFFFFF, // White
	0xFFF5F5F5, // WhiteSmoke
	0xFF4763FF, // Tomato
	0xFFAAB220, // LightSeaGreen
	0xFFEBCE87, // SkyBlue
	0xFF2D52A0, // Sienna
	0xFFB9DAFF, // PeachPuff
	0xFF3F85CD, // Peru
	0xFFCBC0FF, // Pink
	0xFFDDA0DD, // Plum
	0xFFE6E0B0, // PowderBlue
	0xFF800080, // Purple
	0xFFC0C0C0, // Silver
	0xFF0000FF, // Red
	0xFFE16941, // RoyalBlue
	0xFF13458B, // SaddleBrown
	0xFF7280FA, // Salmon
	0xFF60A4F4, // SandyBrown
	0xFF578B2E, // SeaGreen
	0xFFEEF5FF, // SeaShell
	0xFF8F8FBC, // RosyBrown
	0xFF00FFFF, // Yellow
	0xFF7AA0FF, // LightSalmon
	0xFF90EE90, // LightGreen
	0xFF00008B, // DarkRed
	0xFFCC3299, // DarkOrchid
	0xFF008CFF, // DarkOrange
	0xFF2F6B55, // DarkOliveGreen
	0xFF8B008B, // DarkMagenta
	0xFF6BB7BD, // DarkKhaki
	0xFF006400, // DarkGreen
	0xFFA9A9A9, // DarkGray
	0xFF0B86B8, // DarkGoldenrod
	0xFF8B8B00, // DarkCyan
	0xFF8B0000, // DarkBlue
	0xFFFFFF00, // Cyan
	0xFF3C14DC, // Crimson
	0xFFDCF8FF, // Cornsilk
	0xFFED9564, // CornflowerBlue
	0xFF507FFF, // Coral
	0xFF1E69D2, // Chocolate
	0xFFD7EBFA, // AntiqueWhite
	0xFFFFFF00, // Aqua
	0xFFD4FF7F, // Aquamarine
	0xFFFFFFF0, // Azure
	0xFFDCF5F5, // Beige
	0xFFC4E4FF, // Bisque
	0xFF7A96E9, // DarkSalmon
	0xFF000000, // Black
	0xFFFF0000, // Blue
	0xFFE22B8A, // BlueViolet
	0xFF2A2AA5, // Brown
	0xFF87B8DE, // BurlyWood
	0xFFA09E5F, // CadetBlue
	0xFF00FF7F, // Chartreuse
	0xFFCDEBFF, // BlanchedAlmond
	0xFF8FBC8F, // DarkSeaGreen
	0xFF8B3D48, // DarkSlateBlue
	0xFF4F4F2F, // DarkSlateGray
	0xFFB469FF, // HotPink
	0xFF5C5CCD, // IndianRed
	0xFF82004B, // Indigo
	0xFFF0FFFF, // Ivory
	0xFF8CE6F0, // Khaki
	0xFFFAE6E6, // Lavender
	0xFFF0FFF0, // Honeydew
	0xFFF5F0FF, // LavenderBlush
	0xFFCDFAFF, // LemonChiffon
	0xFFE6D8AD, // LightBlue
	0xFF8080F0, // LightCoral
	0xFFFFFFE0, // LightCyan
	0xFFD2FAFA, // LightGoldenrodYellow
	0xFFD3D3D3, // LightGray
	0xFF00FC7C, // LawnGreen
	0xFFC1B6FF, // LightPink
	0xFF2FFFAD, // GreenYellow
	0xFF808080, // Gray
	0xFFD1CE00, // DarkTurquoise
	0xFFD30094, // DarkViolet
	0xFF9314FF, // DeepPink
	0xFFFFBF00, // DeepSkyBlue
	0xFF696969, // DimGray
	0xFFFF901E, // DodgerBlue
	0xFF008000, // Green
	0xFF2222B2, // Firebrick
	0xFF228B22, // ForestGreen
	0xFFFF00FF, // Fuchsia
	0xFFDCDCDC, // Gainsboro
	0xFFFFF8F8, // GhostWhite
	0xFF00D7FF, // Gold
	0xFF20A5DA, // Goldenrod
	0xFFF0FAFF, // FloralWhite
	0xFF32CD9A, // YellowGreen
};
int ReplayData::GetColorValue(const Color& c) { return ColorValuesABGR.at(c); }

template <typename T> const T ReadValue(std::istream& input)
{
	T v;
	input.read((char*)&v, sizeof(T));
	return v;
}

const Transform ReadTransform(std::istream& input)
{
	Transform xForm;
	xForm.translation = ReadValue<Point>(input);
	xForm.rotation = ReadValue<Quaternion>(input);
	return xForm;
}

const Entity ReadEntity(std::istream& input)
{
	Entity entity = {};

	entity.Id = Read7BitEncodedInt(input);
	entity.Name = ReadString(input);
	entity.Path = ReadString(input);
	entity.TypeName = ReadString(input);
	entity.CategoryName = ReadString(input);
	entity.InitialTransform = ReadTransform(input);
	entity.StaticParameters = ReadStringDict(input);
	entity.CreationFrame = Read7BitEncodedInt(input);

	return entity;
}

const std::string ReadString(std::istream& input)
{
	std::string s;
	int stringLength = Read7BitEncodedInt(input);
	s.resize(stringLength);
	input.read((char*)s.c_str(), stringLength);
	return s;
}

const Color ReadColor(std::istream& input)
{
	const int color = Read7BitEncodedInt(input);
	return (Color)color;
}

const std::map<std::string,std::string> ReadStringDict(std::istream& input)
{
	std::map<std::string, std::string> map;
	int count = Read7BitEncodedInt(input);
	while (count-- > 0)
	{
		map[ReadString(input)] = ReadString(input);
	}
	return map;
}

int Read7BitEncodedInt(std::istream& input)
{
	uint32_t result = 0;
	char byteReadJustNow;
	const int32_t MaxBytesWithoutOverflow = 4;
	for (int32_t shift = 0; shift < MaxBytesWithoutOverflow * 7; shift += 7)
	{
		input.read(&byteReadJustNow, 1);
		result |= (byteReadJustNow & 0x7Fu) << shift;
		if (byteReadJustNow <= 0x7Fu)
		{
			return (int)result; // early exit
		}
	}
	input.read(&byteReadJustNow, 1);
	result |= (uint32_t)byteReadJustNow << (MaxBytesWithoutOverflow * 7);
	return (int)result;
}

void ReplayData::Read(std::istream& input)
{
	// Auto-detect compression
	int header = ReadValue<int>(input);
	input.seekg(0);
	if (header == BlockType::ReplayHeader)
	{
		// uncompressed
		ReadInternal(input);
	}
	else
	{
		// compressed

		input.seekg(0, std::ios::end);
		unsigned int compressed_data_size = (unsigned int)input.tellg();
		input.seekg(0, std::ios::beg);

		std::string compressed_data_buffer(compressed_data_size, 0);
		input.read(const_cast<char*>(compressed_data_buffer.c_str()), compressed_data_size);

		//
		// TODO: Dynamic sizing OR streaming
		//
		unsigned int max_decompressed_data_size = 200000000; // As an example (max buffer size)
		std::string decompressed_data_buffer(max_decompressed_data_size, 0);

		Decompressor example_decompressor = Decompressor();
		unsigned int decompressed_data_size = example_decompressor.Feed(const_cast<char*>(compressed_data_buffer.c_str()), compressed_data_size, (unsigned char*) (decompressed_data_buffer.c_str()), max_decompressed_data_size, false);
		decompressed_data_buffer.resize(decompressed_data_size);
		
		std::istringstream iss{ decompressed_data_buffer };
		ReadInternal(iss);
	}
}

void ReplayData::ReadInternal(std::istream& input)
{
	while (!input.eof())
	{
		int _blockType = Read7BitEncodedInt(input);
		BlockType blockType = (BlockType)_blockType;

		if (blockType == BlockType::ReplayHeader)
		{
			// empty
		}
		else if (blockType == BlockType::FrameStep)
		{
			const float totalTime = ReadValue<float>(input);
			frametimes.push_back(totalTime);
			if (abs(totalTime) > framesForTimes.size()) { framesForTimes.push_back((int)frametimes.size()); }
		}
		else
		{
			const int frame = Read7BitEncodedInt(input);
			const int id = Read7BitEncodedInt(input);
			if (blockType == BlockType::EntityDef)
			{
				const Entity entitydef = ReadEntity(input);

				const auto previouslyDefinedEntityElement = entities.find(entitydef.Id);
				if (previouslyDefinedEntityElement != entities.end())
				{
					// Overrides
					Entity& previouslyDefinedEntity = previouslyDefinedEntityElement->second;
					previouslyDefinedEntity.Name = entitydef.Name;
					previouslyDefinedEntity.Path = entitydef.Path;
					previouslyDefinedEntity.CategoryName = entitydef.CategoryName;
					previouslyDefinedEntity.TypeName = entitydef.TypeName;
					previouslyDefinedEntity.InitialTransform = entitydef.InitialTransform;
					previouslyDefinedEntity.StaticParameters = entitydef.StaticParameters;
					previouslyDefinedEntity.RegistrationFrame = entitydef.CreationFrame;
				}
				else
				{
					entities[entitydef.Id] = entitydef;
				}
				entityCategories.insert(entitydef.CategoryName);
			}

			Entity& entity = (id > 0) ? entities[id - 1] : entities[0];

			switch (blockType)
			{
			case BlockType::EntityDef: break;
			case BlockType::EntityUndef:
			{
				entity.DestructionFrame = frame;
				entityLifetimes[entity.Id] = FrameRange{ entity.CreationFrame, frame };
			}
			break;
			case BlockType::EntitySetPos:
			{
				const Point p = ReadValue<Point>(input);
				//if (!last_xforms.TryGetValue(entity, out Transform xform)) { xform = new Transform(); xform.Rotation.W = 1; }
				//xform.Translation = p;
				//EntitySetTransforms.For(entity) ? .AddForBake(frame, xform);
				entity.HasTransforms = true;
				//last_xforms[entity] = xform;
			}
			break;
			case BlockType::EntitySetTransform:
			{
				const Transform xForm = ReadTransform(input);
				//EntitySetTransforms.For(entity) ? .AddForBake(frame, xform);
				entity.HasTransforms = true;
				//last_xforms[entity] = xform;
			}
			break;
			case BlockType::EntityLog:
			{
				const std::string category = ReadString(input);
				std::string msg = ReadString(input);
				std::replace(msg.begin(), msg.end(), '\r', ' '); // newlines stripped
				std::replace(msg.begin(), msg.end(), '\n', ' '); // newlines stripped
				const Color color = ReadColor(input);
				entity.HasLogs = true;
				entity.HasLogsPastFirstFrame |= frame > entity.CreationFrame;
				//LogCategories.Add(category);
				//LogColors.Add(color);
				//LogEntries.AddForBake(frame, (entity, category, msg, color));
				logs.emplace_back(LogEntry{ (int)logs.size(), &entity, frame, GetTimeForFrame(frame), category, msg, color });
				//// Add per entity frame markers
				//if (!LogEntityFrameMarkers.TryGetValue(entity, out var framesWithLogs))
				//{
				//	framesWithLogs = new();
				//	LogEntityFrameMarkers.Add(entity, framesWithLogs);
				//}
				//if (framesWithLogs.Count == 0 || framesWithLogs.Last() != frame) { framesWithLogs.Add(frame); }
			}
			break;
			case BlockType::EntityParameter:
			{
				std::string label = ReadString(input);
				std::string val = ReadString(input);
				entity.HasParameters = true;
				//AddToDynamicPropertiesTable(entity, frame, label, val);
			}
			break;
			case BlockType::EntityValue:
			{
				const std::string label = ReadString(input);
				const float val = ReadValue<float>(input);
				entity.HasNumericParameters = true;
				//EntityDynamicValues.For(entity) ? .AddForBake(frame, (label, val));
			}
			break;
			case BlockType::EntityLine:
			{
				const std::string category = ReadString(input);
				const Point p1 = ReadValue<Point>(input);
				const Point p2 = ReadValue<Point>(input);
				const Color color = ReadColor(input);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Line, color = color, frame = frame, xform = new Transform() { Translation = p1 }, p2 = p2, scale = 1 });
			}
			break;
			case BlockType::EntityCircle:
			{
				const std::string category = ReadString(input);
				const Point center = ReadValue<Point>(input);
				const Point up = ReadValue<Point>(input);
				const float radius = ReadValue<float>(input);
				const Color color = ReadColor(input);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Circle, color = color, frame = frame, xform = new Transform() { Translation = center }, p2 = up, scale = radius });
			}
			break;
			case BlockType::EntitySphere:
			{
				const std::string category = ReadString(input);
				const Point center = ReadValue<Point>(input);
				const float radius = ReadValue<float>(input);
				const Color color = ReadColor(input);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Sphere, color = color, frame = frame, xform = new Transform() { Translation = center }, scale = radius });
			}
			break;
			case BlockType::EntityBox:
			{
				const std::string category = ReadString(input);
				const Transform xForm = ReadTransform(input);
				const Point dimensions = ReadValue<Point>(input);
				const Color color = ReadColor(input);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Box, color = color, frame = frame, xform = xform, p2 = dimensions, scale = 1 });
			}
			break;
			case BlockType::EntityCapsule:
			{
				const std::string category = ReadString(input);
				const Point p1 = ReadValue<Point>(input);
				const Point p2 = ReadValue<Point>(input);
				const float radius = ReadValue<float>(input);
				const Color color = ReadColor(input);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Capsule, color = color, frame = frame, xform = new Transform() { Translation = p1 }, p2 = p2, scale = radius });
			}
			break;
			case BlockType::EntityMesh:
			{
				const std::string category = ReadString(input);
				const int vertexCount = ReadValue<int>(input);
				std::vector<Point> verts;
				verts.resize(vertexCount);
				input.read((char*) &verts[0], sizeof(Point) * vertexCount);
				const Color color = ReadColor(input);
				entity.HasMesh = true;
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Mesh, verts = verts, color = color, frame = frame });
			}
			break;
			default:
				//assert(false);
				break;
			}
		}
	}

	// fill acceleration structures
	for (LogEntry& logEntry : logs)
	{
		logEntry.entity->Logs.push_back(&logEntry);
	}
 }

 float ReplayData::GetTotalTime() const
 {
	 int frameCount = (int)frametimes.size();
	 if (frameCount == 0) return 1.192092896e-07F; //FTL_EPSILON
	 return frametimes.at((size_t)frameCount - 1); // last frame time
 }

 float ReplayData::GetTimeForFrame(int frame) const
 {
	 int frameCount = (int)frametimes.size();
	 if (frameCount == 0) return 0;
	 if (frame < 0) return GetTotalTime();
	 frame = std::clamp(frame, 0, frameCount-1);
	 return frametimes.at(frame);
 }