# pragma once
# include <Siv3D.hpp>
# include "IConfig.hpp"

class ConfigParser
{
public:
	ConfigParser() = default;

	/// @brief JSON パーサーを追加します。
	/// @param dataType 追加する JSON パーサーのデータタイプです。
	/// @param parser パースする関数です。
	void addJSONParser(StringView dataType, std::function<std::unique_ptr<IConfig>(const JSON&)> parser);

	/// @brief path から JSON をパースします。
	/// @param path JSON ファイルの絶対パスです。
	/// @param friendlyPath JSON ファイルの相対パスです。
	/// @return JSON からパースされたデータです。パースに失敗した場合は nullptr を返します。
	[[nodiscard]]
	std::unique_ptr<IConfig> parseJSON(FilePathView path, FilePathView friendlyPath);

private:
	/// @brief dataType と　JSON パーサーのマップです。
	HashTable<String, std::function<std::unique_ptr<IConfig>(const JSON&)>> m_jsonParsers;
};
