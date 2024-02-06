# pragma once
# include <Siv3D.hpp>

/// @brief JSONを型ごとにパースします。
namespace JSONParser
{
	/// @brief `json`からint32に変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したint32を返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	Optional<int32> ReadInt32(const JSON& json, StringView key);

	/// @brief `json`からdoubleに変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したdoubleを返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	Optional<double> ReadDouble(const JSON& json, StringView key);

	/// @brief `json`からVec2に変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したVec2を返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	Optional<Vec2> ReadVec2(const JSON& json, StringView key);

	/// @brief `json`からColorFに変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したColorFを返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	Optional<ColorF> ReadColorF(const JSON& json, StringView key);

	/// @brief `json`からStringに変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したStringを返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	Optional<String> ReadString(const JSON& json, StringView key);

	/// @brief `json`からboolに変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したboolを返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	Optional<bool> ReadBool(const JSON& json, StringView key);
	 
}
