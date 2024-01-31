# include <Siv3D.hpp>
# include "Editor/Editor.hpp"

/// @brief jsonの体裁を型ごとにチェックします。
namespace JsonTypeCheckers
{
	/// @brief doubleの体裁として記述されているか確認します。
	/// @param json keyを持っている json ファイルを渡します。
	/// @param key チェックする key を渡します。
	/// @return 体裁に問題が無ければ true を返します。
	static bool isValidDouble(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return false;
		}

		const auto& vec2 = json[key];
		if (not vec2.contains(U"type") || not vec2[U"type"].isString() || vec2[U"type"] != U"double" ||
			not vec2.contains(U"value") || not vec2[U"value"].isNumber())
		{
			return false;
		}

		return true;
	}

	/// @brief Vec2の体裁として記述されているか確認します。
	/// @param json keyを持っている json ファイルを渡します。
	/// @param key チェックする key を渡します。
	/// @return 体裁に問題が無ければ true を返します。
	static bool isValidVec2(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return false;
		}

		const auto& vec2 = json[key];
		if (not vec2.contains(U"type") || not vec2[U"type"].isString() || vec2[U"type"] != U"Vec2" ||
			not vec2.contains(U"x") || not vec2[U"x"].isNumber() ||
			not vec2.contains(U"y") || not vec2[U"y"].isNumber())
		{
			return false;
		}

		return true;
	}

	/// @brief ColorFの体裁として記述されているか確認します。
	/// @param json keyを持っている json ファイルを渡します。
	/// @param key ColorF の key を渡します。
	/// @return 体裁に問題が無ければ true を返します。
	static bool isValidColorF(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return false;
		}

		const auto& color = json[key];
		if (not color.contains(U"type") || not color[U"type"].isString() || color[U"type"] != U"ColorF" ||
			not color.contains(U"r") || not color[U"r"].isNumber() ||
			not color.contains(U"g") || not color[U"g"].isNumber() ||
			not color.contains(U"b") || not color[U"b"].isNumber() ||
			not color.contains(U"a") || not color[U"a"].isNumber())
		{
			return false;
		}

		return true;
	}

	/// @brief Circleの体裁として記述されているか確認します。
	/// @param json keyを持っている json ファイルを渡します。
	/// @param key チェックする key を渡します。
	/// @return 体裁に問題が無ければ true を返します。
	static bool isValidCircle(const JSON& json, StringView key)
	{
		if (not json.contains(key) || not json[key].isObject())
		{
			return false;
		}

		const auto& circle = json[key];

		if (not circle.contains(U"type") || not circle[U"type"].isString() || circle[U"type"] != U"Circle" ||
			not circle.contains(U"center") || not circle[U"center"].isObject() ||
			not circle.contains(U"radius") || not circle[U"radius"].isObject())
		{
			return false;
		}

		if (not isValidVec2(circle, U"center"))
		{
			return false;
		}

		if (not isValidDouble(circle, U"radius"))
		{
			return false;
		}

		return true;
	}
}

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

		if (not JsonTypeCheckers::isValidColorF(json, U"color"))
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		const auto& color = json[U"color"];

		result.color = ColorF{ color[U"r"].get<double> (),color[U"g"].get<double>() ,color[U"b"].get<double>() ,color[U"a"].get<double>() };
		Editor::ShowSuccess(U"SolidColorBackground のパースに成功しました。");
		return result;
	}
};

struct CircleObject :IConfig
{
	static constexpr StringView DataType = U"circleObject";

	Circle circle{0,0,0};

	[[nodiscard]]
	StringView dataType() const override
	{
		return DataType;
	}

	[[nodiscard]]
	static CircleObject Parse(const JSON& json)
	{
		CircleObject result;

		if (not JsonTypeCheckers::isValidCircle(json, U"circle"))
		{
			Editor::ShowError(U"CircleObject のパースに失敗しました。");
			return result;
		}
		const auto& circle = json[U"circle"];
		const auto& center = circle[U"center"];
		result.circle.center = Vec2{ center[U"x"].get<double>(),center[U"y"].get<double>() };

		const auto& radius = circle[U"radius"];
		result.circle.r = radius[U"value"].get<double>();
		
		Editor::ShowSuccess(U"CircleObject のパースに成功しました。");
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

			//ここから下をjsonの読み込み用として一つの関数にまとめられそう
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
		}

		circleObject.circle.draw();

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
