#pragma once
#include <iosfwd>

enum VRD_Color { AliceBlue, PaleGoldenrod, Orchid, OrangeRed, Orange, OliveDrab, Olive, OldLace, Navy, NavajoWhite, Moccasin, MistyRose, MintCream, MidnightBlue, MediumVioletRed, MediumTurquoise, MediumSpringGreen, MediumSlateBlue, LightSkyBlue, LightSlateGray, LightSteelBlue, LightYellow, Lime, LimeGreen, PaleGreen, Linen, Maroon, MediumAquamarine, MediumBlue, MediumOrchid, MediumPurple, MediumSeaGreen, Magenta, PaleTurquoise, PaleVioletRed, PapayaWhip, SlateGray, Snow, SpringGreen, SteelBlue, Tan, Teal, SlateBlue, Thistle, Transparent, Turquoise, Violet, Wheat, White, WhiteSmoke, Tomato, LightSeaGreen, SkyBlue, Sienna, PeachPuff, Peru, Pink, Plum, PowderBlue, Purple, Silver, Red, RoyalBlue, SaddleBrown, Salmon, SandyBrown, SeaGreen, SeaShell, RosyBrown, Yellow, LightSalmon, LightGreen, DarkRed, DarkOrchid, DarkOrange, DarkOliveGreen, DarkMagenta, DarkKhaki, DarkGreen, DarkGray, DarkGoldenrod, DarkCyan, DarkBlue, Cyan, Crimson, Cornsilk, CornflowerBlue, Coral, Chocolate, AntiqueWhite, Aqua, Aquamarine, Azure, Beige, Bisque, DarkSalmon, Black, Blue, BlueViolet, Brown, BurlyWood, CadetBlue, Chartreuse, BlanchedAlmond, DarkSeaGreen, DarkSlateBlue, DarkSlateGray, HotPink, IndianRed, Indigo, Ivory, Khaki, Lavender, Honeydew, LavenderBlush, LemonChiffon, LightBlue, LightCoral, LightCyan, LightGoldenrodYellow, LightGray, LawnGreen, LightPink, GreenYellow, Gray, DarkTurquoise, DarkViolet, DeepPink, DeepSkyBlue, DimGray, DodgerBlue, Green, Firebrick, ForestGreen, Fuchsia, Gainsboro, GhostWhite, Gold, Goldenrod, FloralWhite, YellowGreen };

typedef struct VRD_Point_s { float x, y, z; } VRD_Point;
typedef struct VRD_Quaternion_s { float x, y, z, w; } VRD_Quaternion;
typedef struct VRD_Transform_s { VRD_Point translation; VRD_Quaternion rotation; } VRD_Transform;
typedef struct VRD_StringDictPair_s { const char* key, * value; } VRD_StringDictPair;

class VrdCaptureReader
{

public:
	void Read(std::istream& input);
private:
	void ReadInternal(std::istream& input);
};