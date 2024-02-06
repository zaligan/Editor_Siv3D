#include "JSONParser.hpp"

//TODO: 各関数で似た処理が登場している場合は共通の処理をまとめる(無名名前空間、static)

/// @brief JSONを型ごとにパースします。
namespace JSONParser
{
	Optional<int32> ReadInt32(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return none;
		}

		const auto& i = json[key];
		if (not i.contains(U"type") || not i[U"type"].isString() || i[U"type"] != U"int" ||
						not i.contains(U"value") || not i[U"value"].isNumber())
		{
			return none;
		}

		return i[U"value"].get<int32>();
	}

	Optional<double> ReadDouble(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return none;
		}

		const auto& d = json[key];
		if (not d.contains(U"type") || not d[U"type"].isString() || d[U"type"] != U"double" ||
			not d.contains(U"value") || not d[U"value"].isNumber())
		{
			return none;
		}

		return d[U"value"].get<double>();
	}

	Optional<Vec2> ReadVec2(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return none;
		}

		const auto& vec2 = json[key];
		if (not vec2.contains(U"type") || not vec2[U"type"].isString() || vec2[U"type"] != U"Vec2" ||
			not vec2.contains(U"x") || not vec2[U"x"].isNumber() ||
			not vec2.contains(U"y") || not vec2[U"y"].isNumber())
		{
			return none;
		}

		return Vec2{ vec2[U"x"].get<double>(),vec2[U"y"].get<double>() };
	}

	Optional<ColorF> ReadColorF(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return none;
		}

		//ColorF の体裁で json ファイルが記述されているか調べる。
		const auto& color = json[key];
		if (not color.contains(U"type") || not color[U"type"].isString() || color[U"type"] != U"ColorF" ||
			not color.contains(U"r") || not color[U"r"].isNumber() ||
			not color.contains(U"g") || not color[U"g"].isNumber() ||
			not color.contains(U"b") || not color[U"b"].isNumber())
		{
			return none;
		}

		//alpha 成分を記述していなかった場合 1.0 にする
		double alpha = 1.0;
		if (color.contains(U"a") && color[U"a"].isNumber())
		{
			alpha = color[U"a"].get<double>();
		}

		return ColorF{ color[U"r"].get<double>(),color[U"g"].get<double>() ,color[U"b"].get<double>() ,alpha };
	}

	Optional<String> ReadString(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return none;
		}

		const auto& s = json[key];
		if (not s.contains(U"type") || not s[U"type"].isString() || s[U"type"] != U"String" ||
						not s.contains(U"value") || not s[U"value"].isString())
		{
			return none;
		}

		return s[U"value"].getString();
	}

	Optional<bool> ReadBool(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return none;
		}

		const auto& b = json[key];
		if (not b.contains(U"type") || not b[U"type"].isString() || b[U"type"] != U"bool" ||
									not b.contains(U"value") || not b[U"value"].isBool())
		{
			return none;
		}

		return b[U"value"].get<bool>();
	}
}
