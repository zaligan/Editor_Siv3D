# include "ConfigParser.hpp"
# include "Editor.hpp"

namespace
{
	/// @brief path から JSON をロードします。
	/// @param path JSON ファイルの絶対パスです。
	/// @param friendlyPath JSON ファイルの相対パスです。
	/// @return JSON とデータタイプのペアを返します。ロードに失敗した場合は空のペアを返します。
	[[nodiscard]]
	static std::pair<JSON, String> LoadConfigJSON(const FilePathView path, const FilePathView friendlyPath)
	{
		Editor::ShowInfo(U"config ファイル`{}`を JSON としてロードします"_fmt(friendlyPath));

		const JSON json = JSON::Load(path);

		if (not json)
		{
			Editor::ShowError(U"config　ファイル`{}`のロードに失敗しました（不正なJSON）。"_fmt(friendlyPath));
			return{};
		}

		if (not json.contains(U"dataType") || not json[U"dataType"].isString())
		{
			Editor::ShowError(U"config ファイル`{}`: dataTypeがないか不正です。"_fmt(friendlyPath));
			return{};
		}

		const String dataType = json[U"dataType"].getString();

		Editor::ShowSuccess(U"データタイプは`{}`です。"_fmt(dataType));

		return { json,dataType };
	}
}

void ConfigParser::addJSONParser(StringView dataType, std::function<std::unique_ptr<IConfig>(const JSON&)> parser)
{
	m_jsonParsers[dataType] = parser;
}

std::unique_ptr<IConfig> ConfigParser::parseJSON(FilePathView path, FilePathView friendlyPath)
{
	// path から JSON をロードします。
	const auto [json, dataType] = LoadConfigJSON(path, friendlyPath);

	// ロードした JSON のデータタイプをもとにパーサーを呼び出します。
	if (auto it = m_jsonParsers.find(dataType);(it == m_jsonParsers.end()))
	{
		Editor::ShowError(U"データタイプ`{}`のパーサーが登録されていません。"_fmt(dataType));
		return nullptr;
	}
	else
	{
		if (auto pConfig = it->second(json))
		{
			Editor::ShowSuccess(U"データタイプ`{}`のパースに成功しました。"_fmt(dataType));
			return pConfig;
		}
		else
		{
			Editor::ShowError(U"データタイプ`{}`のパースに失敗しました。"_fmt(dataType));
			return nullptr;
		}
	}
}
