# include <Siv3D.hpp>
# include "Editor/Editor.hpp"

/// @brief jsonの体裁を型ごとにチェックします。
namespace JsonConvertTypes
{
	/// @brief json から double に変換します。
	/// @param json keyを持っている json ファイルを渡します。
	/// @param key 変換したい double の key を渡します。
	/// @return 変換した double を返します。失敗した場合 0 を返します。
	static double jsonToDouble(const JSON& json, StringView key)
	{
		double result = 0.0;

		if (not json.contains(key) || not json[key].isObject())
		{
			Editor::ShowError(U"json を double に変換できませんでした。");
			return result;
		}

		const auto& d = json[key];
		if (not d.contains(U"type") || not d[U"type"].isString() || d[U"type"] != U"double" ||
			not d.contains(U"value") || not d[U"value"].isNumber())
		{
			Editor::ShowError(U"json を double に変換できませんでした。");
			return result;
		}

		result = d[U"value"].get<double>();

		return result;
	}

	/// @brief json から Vec2 に変換します。
	/// @param json keyを持っている json ファイルを渡します。
	/// @param key 変換したい Vec2 の key を渡します。
	/// @return 変換した Vec2 を返します。失敗した場合 {0,0} を返します。
	static Vec2 jsonToVec2(const JSON& json, StringView key)
	{
		Vec2 result{ 0,0 };

		if (not json.contains(key) || not json[key].isObject())
		{
			Editor::ShowError(U"json を Vec2 に変換できませんでした。");
			return result;
		}

		const auto& vec2 = json[key];
		if (not vec2.contains(U"type") || not vec2[U"type"].isString() || vec2[U"type"] != U"Vec2" ||
			not vec2.contains(U"x") || not vec2[U"x"].isNumber() ||
			not vec2.contains(U"y") || not vec2[U"y"].isNumber())
		{
			Editor::ShowError(U"json を Vec2 に変換できませんでした。");
			return result;
		}

		result = Vec2{ vec2[U"x"].get<double>(),vec2[U"y"].get<double>() };

		return result;
	}

	/// @brief json から ColorF に変換します。
	/// @param json keyを持っている json ファイルを渡します。
	/// @param key 変換したい ColorF の key を渡します。
	/// @return 変換した ColorF を返します。失敗した場合`白`を返します。
	static ColorF jsonToColorF(const JSON& json, StringView key)
	{
		ColorF result{ 1.0,1.0, 1.0, 1.0 };

		if (not json.contains(key) || not json[key].isObject())
		{
			Editor::ShowError(U"json を ColorF に変換できませんでした。");
			return result;
		}

		//ColorF の体裁で json ファイルが記述されているか調べる。
		const auto& color = json[key];
		if (not color.contains(U"type") || not color[U"type"].isString() || color[U"type"] != U"ColorF" ||
			not color.contains(U"r") || not color[U"r"].isNumber() ||
			not color.contains(U"g") || not color[U"g"].isNumber() ||
			not color.contains(U"b") || not color[U"b"].isNumber())
		{
			Editor::ShowError(U"json を ColorF に変換できませんでした。");
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

		result.color = JsonConvertTypes::jsonToColorF(json, U"color");
		
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

		result.center = JsonConvertTypes::jsonToVec2(json, U"center");

		result.radius = JsonConvertTypes::jsonToDouble(json, U"radius");

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
