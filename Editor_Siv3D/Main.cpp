# include <Siv3D.hpp>
# include "Editor/Editor.hpp"
# include "Editor/JSONParser.hpp"

struct IConfig
{
	virtual ~IConfig() = default;

	[[nodiscard]]
	virtual StringView dataType() const = 0;
};

struct SolidColorBackground : IConfig
{
	static constexpr StringView DataType = U"solidColorBackgrond";

	ColorF color{ 1.0,1.0,1.0,1.0 };

	[[nodiscard]]
	StringView dataType() const override
	{
		return DataType;
	}

	[[nodiscard]]
	static SolidColorBackground Parse(const JSON& json)
	{
		SolidColorBackground result;

		if (const auto color = JSONParser::JSONParseColorF(json, U"color"))
		{
			Editor::ShowSuccess(U"SolidColorBackground のパースに成功しました。");
			result.color = *color;
		}
		else
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
		}

		return result;
	}
};

struct CircleObject :IConfig
{
	static constexpr StringView DataType = U"circleObject";

	Vec2 center{ 0,0 };

	double radius = 0;

	[[nodiscard]]
	StringView dataType() const override
	{
		return DataType;
	}

	[[nodiscard]]
	static CircleObject Parse(const JSON& json)
	{
		CircleObject result;

		if (const auto center = JSONParser::JSONParseVec2(json, U"center"))
		{
			result.center = *center;
		}
		else
		{
			Editor::ShowError(U"CircleObject のパースに失敗しました。");
			return result;
		}

		if (const auto radius = JSONParser::JSONParseDouble(json, U"radius"))
		{
			Editor::ShowSuccess(U"CircleObject のパースに成功しました。");
			result.radius = *radius;
		}
		else
		{
			Editor::ShowError(U"CircleObject のパースに失敗しました。");
		}

		return result;
	}
};

struct TestParsePrint :IConfig
{
	static constexpr StringView DataType = U"parseTest";

	int32 loopCnt = 0;

	String text = U"fail";

	[[nodiscard]]
	StringView dataType() const override
	{
		return DataType;
	}

	[[nodiscard]]
	static TestParsePrint Parse(const JSON& json)
	{
		TestParsePrint result;

		if (const auto loopCnt = JSONParser::JSONParseInt32(json, U"count"))
		{
			result.loopCnt = *loopCnt;
		}
		else
		{
			Editor::ShowError(U"TestParsePrint のパースに失敗しました。");
			return result;
		}

		if (const auto text = JSONParser::JSONParseString(json, U"print"))
		{
			Editor::ShowSuccess(U"TestParsePrint のパースに成功しました。");
			result.text = *text;
		}
		else
		{
			Editor::ShowError(U"TestParsePrint のパースに失敗しました。");
		}

		return result;
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

	CircleObject circleObject;

	TestParsePrint testParsePrint;

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
				const SolidColorBackground solidColorBackground =SolidColorBackground::Parse(json);
				Scene::SetBackground(solidColorBackground.color);
			}
			else if (dataType == CircleObject::DataType)
			{
				circleObject = CircleObject::Parse(json);
			}
			else if (dataType == TestParsePrint::DataType)
			{
				testParsePrint = TestParsePrint::Parse(json);
			}
		}

		Circle {circleObject.center,circleObject.radius}.draw();

		if (MouseL.down())
		{
			for (int32 i = 0; i < testParsePrint.loopCnt; ++i)
			{
				Print << testParsePrint.text;
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
