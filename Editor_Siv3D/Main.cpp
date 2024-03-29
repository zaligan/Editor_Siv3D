﻿# include <Siv3D.hpp>
# include "Editor/Editor.hpp"
# include "Editor/JSONParser.hpp"
# include "Editor/IConfig.hpp"
# include "Editor/ConfigParser.hpp"

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
			return std::make_unique<SolidColorBackground>(*color);
		}
		else
		{
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
			return std::make_unique<CircleObject>(*center, *radius);
		}
		else
		{
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
			return std::make_unique<TestParsePrint>(*loopCount, *text);
		}
		else
		{
			return nullptr;
		}
	}
};




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

	
	ConfigParser configParser;
	configParser.addJSONParser(SolidColorBackground::DataType, &SolidColorBackground::Parse);
	configParser.addJSONParser(CircleObject::DataType, &CircleObject::Parse);
	configParser.addJSONParser(TestParsePrint::DataType, &TestParsePrint::Parse);


	while (System::Update())
	{
		editor.update();

		//変更のあった config ディレクトリを全て調べます。
		for (const auto& changedConfigFile : editor.retrieveChangedConfigFiles())
		{
			const FilePath friendlyPath = FileSystem::RelativePath(changedConfigFile);
			Editor::ShowInfo(U"configファイル`{}`が更新されました。"_fmt(friendlyPath));

			if (const String extension = FileSystem::Extension(changedConfigFile); (extension == U"json"))
			{
				if (auto pConfig = configParser.parseJSON(changedConfigFile, friendlyPath))
				{
					configs[pConfig->dataType()] = std::move(pConfig);
				}
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
