# pragma once
# include <Siv3D.hpp>
# include "IConfig.hpp"

class ConfigParser
{
public:
	ConfigParser() = default;

	void addJSONParser(StringView dataType, std::function<std::unique_ptr<IConfig>(const JSON&)> parser);

	[[nodiscard]]
	std::unique_ptr<IConfig> parseJSON(FilePathView path, FilePathView friendlyPath);

private:
	HashTable<String, std::function<std::unique_ptr<IConfig>(const JSON&)>> m_jsonParsers;
};
