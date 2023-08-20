#include "VrdCaptureReader.hpp"

#include "inflatecpp\decompressor.h"
#include <sstream>

#include <assert.h>

#include <list>
#include <vector>
#include <map>

// fwd decl
struct Entity;
struct Transform;
struct Point;
struct Quaternion;
const Entity ReadEntity(std::istream& input);
const std::string ReadString(std::istream& input);
const std::map<std::string, std::string> ReadStringDict(std::istream& input);
int Read7BitEncodedInt(std::istream& input);


enum VRD_BlockType
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

struct Point { float X, Y, Z; };

struct Quaternion
{
	float X, Y, Z, W;
	//Quaternion Identity = new Quaternion(){ W = 1 };
};

struct Transform
{
	Point Translation;
	Quaternion Rotation;

	//public static readonly Transform Identity = new Transform(){ Rotation = Quaternion.Identity };
};

struct Entity
{
	int Id;
	std::string Name;
	std::string Path;
	std::string TypeName;
	std::string CategoryName;
	Transform InitialTransform;
	std::map<std::string, std::string> StaticParameters;
	int CreationFrame;

	int RegistrationFrame;
	bool HasTransforms;
	bool HasLogs;
	bool HasLogsPastFirstFrame;
	bool HasDraws;
	bool HasMesh;
	bool HasParameters;
	bool HasNumericParameters;
};

template <typename T> const T ReadValue(std::istream& input)
{
	T v;
	input.read((char*)&v, sizeof(T));
	return v;
}

const Entity ReadEntity(std::istream& input)
{
	Entity entity = {};

	entity.Id = Read7BitEncodedInt(input);
	entity.Name = ReadString(input);
	entity.Path = ReadString(input);
	entity.TypeName = ReadString(input);
	entity.CategoryName = ReadString(input);
	entity.InitialTransform = ReadValue<Transform>(input);
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

static std::list<Entity> s_entities;

void VrdCaptureReader::Read(std::istream& input)
{
	// Auto-detect compression
	int header = ReadValue<int>(input);
	input.seekg(0);
	if (header == VRD_BlockType::ReplayHeader)
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

		// TODO: Dynamic sizing
		unsigned int max_decompressed_data_size = 200000000; // As an example (max buffer size)
		std::string decompressed_data_buffer(max_decompressed_data_size, 0);

		Decompressor example_decompressor = Decompressor();
		unsigned int decompressed_data_size = example_decompressor.Feed(const_cast<char*>(compressed_data_buffer.c_str()), compressed_data_size, (unsigned char*) (decompressed_data_buffer.c_str()), max_decompressed_data_size, false);
		decompressed_data_buffer.resize(decompressed_data_size);
		
		std::istringstream iss{ decompressed_data_buffer };
		ReadInternal(iss);
	}
}

void VrdCaptureReader::ReadInternal(std::istream& input)
{
	std::list<float> frametimes;
	std::list<int> framesForTimes;

	while (true)
	{
		int blockType = Read7BitEncodedInt(input);
		if (blockType == VRD_BlockType::ReplayHeader)
		{
			// empty
		}
		else if (blockType == VRD_BlockType::FrameStep)
		{
			float totalTime = ReadValue<float>(input);
			frametimes.push_back(totalTime);
			if (abs(totalTime) > framesForTimes.size()) { framesForTimes.push_back((int)frametimes.size()); }
		}
		else
		{
			int frame = Read7BitEncodedInt(input);
			int id = Read7BitEncodedInt(input);
			if (blockType == VRD_BlockType::EntityDef)
			{
				const Entity entity = ReadEntity(input);
				s_entities.push_back(entity);

				//var previouslyDefinedEntity = Entities.FirstOrDefault(x = > x.Id == entitydef.Id) as EntityEx;
				//if (previouslyDefinedEntity != null)
				//{
				//	// Overrides
				//	previouslyDefinedEntity.Name = entitydef.Name;
				//	previouslyDefinedEntity.Path = entitydef.Path;
				//	previouslyDefinedEntity.CategoryName = entitydef.CategoryName;
				//	previouslyDefinedEntity.TypeName = entitydef.TypeName;
				//	previouslyDefinedEntity.InitialTransform = entitydef.InitialTransform;
				//	previouslyDefinedEntity.StaticParameters = entitydef.StaticParameters;
				//	previouslyDefinedEntity.RegistrationFrame = entitydef.CreationFrame;
				//}
				//else
				//{
				//	Entities.Add(entitydef);
				//}
				//EntityCategories.Add(entitydef.CategoryName);
			}

			switch (blockType)
			{
			case VRD_BlockType::EntityDef: break;
			case VRD_BlockType::EntityUndef:
			{
				//if (entity != null) EntityLifeTimes.Add(entity, new FrameRange(){ Start = entity.CreationFrame, End = frame });
			}
			break;
			case VRD_BlockType::EntitySetPos:
			{
				//reader.Read(out Point p);
				//if (!last_xforms.TryGetValue(entity, out Transform xform)) { xform = new Transform(); xform.Rotation.W = 1; }
				//xform.Translation = p;
				//EntitySetTransforms.For(entity) ? .AddForBake(frame, xform);
				//entity.HasTransforms = true;
				//last_xforms[entity] = xform;
			}
			break;
			case VRD_BlockType::EntitySetTransform:
			{
				//reader.Read(out Transform xform);
				//EntitySetTransforms.For(entity) ? .AddForBake(frame, xform);
				//entity.HasTransforms = true;
				//last_xforms[entity] = xform;
			}
			break;
			case VRD_BlockType::EntityLog:
			{
				//string category = reader.ReadString();
				//string msg = reader.ReadString();
				//msg = msg.Replace('\r', ' '); // newlines stripped
				//msg = msg.Replace('\n', ' '); // newlines stripped
				//reader.Read(out Color color);
				//entity.HasLogs = true;
				//entity.HasLogsPastFirstFrame |= frame > entity.CreationFrame;
				//LogCategories.Add(category);
				//LogColors.Add(color);
				//LogEntries.AddForBake(frame, (entity, category, msg, color));

				//// Add per entity frame markers
				//if (!LogEntityFrameMarkers.TryGetValue(entity, out var framesWithLogs))
				//{
				//	framesWithLogs = new();
				//	LogEntityFrameMarkers.Add(entity, framesWithLogs);
				//}
				//if (framesWithLogs.Count == 0 || framesWithLogs.Last() != frame) { framesWithLogs.Add(frame); }
			}
			break;
			case VRD_BlockType::EntityParameter:
			{
				//string label = reader.ReadString();
				//string val = reader.ReadString();
				//entity.HasParameters = true;
				//AddToDynamicPropertiesTable(entity, frame, label, val);
			}
			break;
			case VRD_BlockType::EntityValue:
			{
				//string label = reader.ReadString();
				//float val = reader.ReadSingle();
				//entity.HasNumericParameters = true;
				//EntityDynamicValues.For(entity) ? .AddForBake(frame, (label, val));
			}
			break;
			case VRD_BlockType::EntityLine:
			{
				//string category = reader.ReadString();
				//reader.Read(out Point p1);
				//reader.Read(out Point p2);
				//reader.Read(out Color color);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Line, color = color, frame = frame, xform = new Transform() { Translation = p1 }, p2 = p2, scale = 1 });
			}
			break;
			case VRD_BlockType::EntityCircle:
			{
				//string category = reader.ReadString();
				//reader.Read(out Point center);
				//reader.Read(out Point up);
				//float radius = reader.ReadSingle();
				//reader.Read(out Color color);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Circle, color = color, frame = frame, xform = new Transform() { Translation = center }, p2 = up, scale = radius });
			}
			break;
			case VRD_BlockType::EntitySphere:
			{
				//string category = reader.ReadString();
				//reader.Read(out Point center);
				//float radius = reader.ReadSingle();
				//reader.Read(out Color color);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Sphere, color = color, frame = frame, xform = new Transform() { Translation = center }, scale = radius });
			}
			break;
			case VRD_BlockType::EntityBox:
			{
				//string category = reader.ReadString();
				//reader.Read(out Transform xform);
				//reader.Read(out Point dimensions);
				//reader.Read(out Color color);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Box, color = color, frame = frame, xform = xform, p2 = dimensions, scale = 1 });
			}
			break;
			case VRD_BlockType::EntityCapsule:
			{
				//string category = reader.ReadString();
				//reader.Read(out Point p1);
				//reader.Read(out Point p2);
				//float radius = reader.ReadSingle();
				//reader.Read(out Color color);
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Capsule, color = color, frame = frame, xform = new Transform() { Translation = p1 }, p2 = p2, scale = radius });
			}
			break;
			case VRD_BlockType::EntityMesh:
			{
				std::string category = ReadString(input);
				int vertexCount = ReadValue<int>(input);
				std::vector<Point> verts;
				verts.resize(vertexCount);
				input.read((char*) &verts[0], sizeof(Point) * vertexCount);
				int color = ReadValue<int>(input);
				//entity.HasMesh = true;
				//AddDrawCommand(frame, new EntityDrawCommand(){ entity = entity, category = category, type = EntityDrawCommandType.Mesh, verts = verts, color = color, frame = frame });
			}
			break;
			default:
				assert(false);
				break;
			}
		}
	}
 }