# pragma once
# include <Siv3D.hpp>

struct IConfig
{
	virtual ~IConfig() = default;

	[[nodiscard]]
	virtual StringView dataType() const = 0;
};

template <class ConfigType>
[[nodiscard]]
ConfigType* GetConfig(const HashTable<String, std::unique_ptr<IConfig>>& configs)
{
	if (auto it = configs.find(ConfigType::DataType); (it != configs.end()))
	{
		//ConfigTypeから作られたポインタでない場合nullptrになる
		return dynamic_cast<ConfigType*>(it->second.get());
	}

	return nullptr;
}

