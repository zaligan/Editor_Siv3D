# include <Siv3D.hpp>
# include "Editor/Editor.hpp"

/// @brief JSONを型ごとにパースします。
namespace JSONParser
{
	/// @brief `json`からdoubleに変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したdoubleを返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	static Optional<double> JSONParseDouble(const JSON& json, StringView key)
	{
		Optional<double> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		const auto& d = json[key];
		if (not d.contains(U"type") || not d[U"type"].isString() || d[U"type"] != U"double" ||
			not d.contains(U"value") || not d[U"value"].isNumber())
		{
			return result;
		}

		result = d[U"value"].get<double>();

		return result;
	}

	/// @brief `json`からVec2に変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したVec2を返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	static Optional<Vec2> JSONParseVec2(const JSON& json, StringView key)
	{
		Optional<Vec2> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		const auto& vec2 = json[key];
		if (not vec2.contains(U"type") || not vec2[U"type"].isString() || vec2[U"type"] != U"Vec2" ||
			not vec2.contains(U"x") || not vec2[U"x"].isNumber() ||
			not vec2.contains(U"y") || not vec2[U"y"].isNumber())
		{
			return result;
		}

		result = Vec2{ vec2[U"x"].get<double>(),vec2[U"y"].get<double>() };

		return result;
	}
	
	/// @brief `json`からColorFに変換します。
	/// @param json `key`を持っている`json`ファイルを渡します。
	/// @param key 変換したい`key`を渡します。
	/// @return 変換したColorFを返します。失敗した場合、無効値を返します。
	[[nodiscard]]
	static Optional<ColorF> JSONParseColorF(const JSON& json, StringView key)
	{
		Optional<ColorF> result;

		if (not json.contains(key) || not json[key].isObject())
		{
			return result;
		}

		//ColorF の体裁で json ファイルが記述されているか調べる。
		const auto& color = json[key];
		if (not color.contains(U"type") || not color[U"type"].isString() || color[U"type"] != U"ColorF" ||
			not color.contains(U"r") || not color[U"r"].isNumber() ||
			not color.contains(U"g") || not color[U"g"].isNumber() ||
			not color.contains(U"b") || not color[U"b"].isNumber())
		{
			return result;
		}

		//alpha 成分を記述していなかった場合 1.0 にする
		double alpha = 1.0;
		if (color.contains(U"a") && color[U"a"].isNumber())
		{
			alpha = color[U"a"].get<double>();
		}

		result = ColorF{ color[U"r"].get<double>(),color[U"g"].get<double>() ,color[U"b"].get<double>() ,alpha};

		return result;
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

		Circle {circleObject.center,circleObject.radius}.draw();

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
