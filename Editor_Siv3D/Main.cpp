# include <Siv3D.hpp>
# include "Editor/Editor.hpp"
# include "Editor/JSONParser.hpp"

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

struct SolidColorBackground : IConfig
{
	static constexpr StringView DataType = U"solidColorBackgrond";

	ColorF color{ 1.0,1.0,1.0,1.0 };

	SolidColorBackground() = default;

	explicit SolidColorBackground(const ColorF& color)
		:color(color) {}

	[[nodiscard]]
	StringView dataType() const override
	{
		return DataType;
	}

	[[nodiscard]]
	static std::unique_ptr<SolidColorBackground> Parse(const JSON& json)
	{
		if (const auto color = JSONParser::ReadColorF(json, U"color"))
		{
			Editor::ShowSuccess(U"SolidColorBackground のパースに成功しました。");
			return std::make_unique<SolidColorBackground>(*color);
		}
		else
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return nullptr;
		}
	}
};

struct CircleObject :IConfig
{
	static constexpr StringView DataType = U"circleObject";

	Vec2 center{ 0,0 };

	double radius = 0;

	CircleObject() = default;

	CircleObject(const Vec2& center, double radius)
		:center(center), radius(radius) {}

	[[nodiscard]]
	StringView dataType() const override
	{
		return DataType;
	}

	[[nodiscard]]
	static std::unique_ptr<CircleObject> Parse(const JSON& json)
	{
		const auto center = JSONParser::ReadVec2(json, U"center");
		const auto radius = JSONParser::ReadDouble(json, U"radius");

		if (center && radius)
		{
			Editor::ShowSuccess(U"CircleObject のパースに成功しました。");
			return std::make_unique<CircleObject>(*center, *radius);
		}
		else
		{
			Editor::ShowError(U"CircleObject のパースに失敗しました。");
			return nullptr;
		}
	}
};

struct TestParsePrint :IConfig
{
	static constexpr StringView DataType = U"parseTest";

	int32 loopCount = 0;

	String text = U"fail";

	TestParsePrint() = default;

	TestParsePrint(int32 loopCount, const String& text)
		:loopCount(loopCount), text(text) {}

	[[nodiscard]]
	StringView dataType() const override
	{
		return DataType;
	}

	[[nodiscard]]
	static std::unique_ptr<TestParsePrint> Parse(const JSON& json)
	{
		const auto loopCount = JSONParser::ReadInt32(json, U"count");
		const auto text = JSONParser::ReadString(json, U"print");

		if (loopCount && text)
		{
			Editor::ShowSuccess(U"TestParsePrint のパースに成功しました。");
			return std::make_unique<TestParsePrint>(*loopCount, *text);
		}
		else
		{
			Editor::ShowError(U"TestParsePrint のパースに失敗しました。");
			return nullptr;
		}
	}
};

[[nodiscard]]
std::pair<JSON, String> LoadConfigJSON(const FilePathView path, const FilePathView friendlyPath)
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


void Main()
{
	Window::Resize(1280, 720);

	//Editorの準備をします
	Editor editor;
	if (not editor.init())
	{
		throw Error{ U"Editorの初期化に失敗しました" };
	}

	if (not editor.prepareConfigDirectory())
	{
		throw Error{ U"configディレクトリの準備に失敗しました" };
	}

	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	HashTable<String, std::unique_ptr<IConfig>> configs;

	
	HashTable<String, std::function<std::unique_ptr<IConfig>(const JSON&)>> jsonParsers;
	jsonParsers.emplace(SolidColorBackground::DataType, &SolidColorBackground::Parse);
	jsonParsers.emplace(CircleObject::DataType, &CircleObject::Parse);
	jsonParsers.emplace(TestParsePrint::DataType, &TestParsePrint::Parse);


	while (System::Update())
	{
		editor.update();

		//変更のあった config ディレクトリを全て調べます。
		for (const auto& changedConfigFile : editor.retrieveChangedConfigFiles())
		{
			const FilePath friendlyPath = FileSystem::RelativePath(changedConfigFile);
			Editor::ShowInfo(U"configファイル`{}`が更新されました。"_fmt(friendlyPath));

			const String extension = FileSystem::Extension(changedConfigFile);
			if (extension != U"json")
			{
				continue;
			}

			const auto [json, dataType] = LoadConfigJSON(changedConfigFile, friendlyPath);

			if (dataType == SolidColorBackground::DataType)
			{
				configs[dataType] = SolidColorBackground::Parse(json);				
			}
			else if (dataType == CircleObject::DataType)
			{
				configs[dataType] = CircleObject::Parse(json);
			}
			else if (dataType == TestParsePrint::DataType)
			{
				configs[dataType] = TestParsePrint::Parse(json);
			}
		}

		if (auto p = GetConfig<SolidColorBackground>(configs))
		{
			Scene::SetBackground(p->color);
		}

		if (auto p = GetConfig<CircleObject>(configs))
		{
			Circle{ p->center,p->radius }.draw();
		}

		if (auto p = GetConfig<TestParsePrint>(configs))
		{
			if (MouseR.down())
			{
				for (int32 i = 0; i < p->loopCount; ++i)
				{
					Print << p->text;
				}
			}
		}

		//通知用ボタンを作成します
		if (SimpleGUI::Button(U"verbose", Vec2{ 1100, 40 }, 160))
		{
			Editor::ShowVerbose(U"verbose");
		}

		if (SimpleGUI::Button(U"information", Vec2{ 1100, 80 }, 160))
		{
			Editor::ShowInfo(U"information");
		}

		if (SimpleGUI::Button(U"success", Vec2{ 1100, 120 }, 160))
		{
			Editor::ShowSuccess(U"success");
		}

		if (SimpleGUI::Button(U"warning", Vec2{ 1100, 160 }, 160))
		{
			Editor::ShowWarning(U"warning");
		}

		if (SimpleGUI::Button(U"error", Vec2{ 1100, 200 }, 160))
		{
			Editor::ShowError(U"error");
		}
	}
}
