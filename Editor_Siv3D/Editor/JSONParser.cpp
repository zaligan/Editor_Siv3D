#include "JSONParser.hpp"

/// @brief JSONを型ごとにパースします。
namespace JSONParser
{
	Optional<int32> JSONParseInt32(const JSON& json, StringView key)
	{
		Optional<int32> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		const auto& i = json[key];
		if (not i.contains(U"type") || not i[U"type"].isString() || i[U"type"] != U"int" ||
						not i.contains(U"value") || not i[U"value"].isNumber())
		{
			return result;
		}

		result = i[U"value"].get<int32>();

		return result;
	}

	Optional<double> JSONParseDouble(const JSON& json, StringView key)
	{
		Optional<double> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		const auto& d = json[key];
		if (not d.contains(U"type") || not d[U"type"].isString() || d[U"type"] != U"double" ||
			not d.contains(U"value") || not d[U"value"].isNumber())
		{
			return result;
		}

		result = d[U"value"].get<double>();

		return result;
	}

	Optional<Vec2> JSONParseVec2(const JSON& json, StringView key)
	{
		Optional<Vec2> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		const auto& vec2 = json[key];
		if (not vec2.contains(U"type") || not vec2[U"type"].isString() || vec2[U"type"] != U"Vec2" ||
			not vec2.contains(U"x") || not vec2[U"x"].isNumber() ||
			not vec2.contains(U"y") || not vec2[U"y"].isNumber())
		{
			return result;
		}

		result = Vec2{ vec2[U"x"].get<double>(),vec2[U"y"].get<double>() };

		return result;
	}

	Optional<ColorF> JSONParseColorF(const JSON& json, StringView key)
	{
		Optional<ColorF> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		//ColorF の体裁で json ファイルが記述されているか調べる。
		const auto& color = json[key];
		if (not color.contains(U"type") || not color[U"type"].isString() || color[U"type"] != U"ColorF" ||
			not color.contains(U"r") || not color[U"r"].isNumber() ||
			not color.contains(U"g") || not color[U"g"].isNumber() ||
			not color.contains(U"b") || not color[U"b"].isNumber())
		{
			return result;
		}

		//alpha 成分を記述していなかった場合 1.0 にする
		double alpha = 1.0;
		if (color.contains(U"a") && color[U"a"].isNumber())
		{
			alpha = color[U"a"].get<double>();
		}

		result = ColorF{ color[U"r"].get<double>(),color[U"g"].get<double>() ,color[U"b"].get<double>() ,alpha };

		return result;
	}

	Optional<String> JSONParseString(const JSON& json, StringView key)
	{
		Optional<String> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		const auto& s = json[key];
		if (not s.contains(U"type") || not s[U"type"].isString() || s[U"type"] != U"String" ||
						not s.contains(U"value") || not s[U"value"].isString())
		{
			return result;
		}

		result = s[U"value"].getString();

		return result;
	}
}
