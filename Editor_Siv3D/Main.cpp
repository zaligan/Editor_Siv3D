# include <Siv3D.hpp>
# include "Editor/Editor.hpp"

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
		if (not json.contains(U"color") || not json[U"color"].isObject())
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		const auto& color = json[U"color"];

		if (not color.contains(U"type") || not color[U"type"].isString())
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		if (color[U"type"] != U"ColorF")
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		if (not color.contains(U"r") || not color[U"r"].isNumber())
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		if (not color.contains(U"g") || not color[U"g"].isNumber())
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		if (not color.contains(U"b") || not color[U"b"].isNumber())
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		if (not color.contains(U"a") || not color[U"a"].isNumber())
		{
			Editor::ShowError(U"SolidColorBackground のパースに失敗しました。");
			return result;
		}

		result.color = ColorF{ color[U"r"].get<double> (),color[U"g"].get<double>() ,color[U"b"].get<double>() ,color[U"a"].get<double>() };
		Editor::ShowSuccess(U"SolidColorBackground のパースに成功しました。");
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

		{
			if (not json.contains(U"center") || not json[U"center"].isObject())
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			const auto& center = json[U"center"];

			if (not center.contains(U"type") || not center[U"type"].isString())
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			if (center[U"type"].getString() != U"Vec2")
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			if (not center.contains(U"x") || not center[U"x"].isNumber())
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			if (not center.contains(U"y") || not center[U"y"].isNumber())
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			result.center = Vec2{ center[U"x"].get<double>(),center[U"y"].get<double>() };
		}

		{
			if (not json.contains(U"radius") || not json[U"radius"].isObject())
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			const auto& radius = json[U"radius"];

			if (not radius.contains(U"type") || not radius[U"type"].isString())
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			if (radius[U"type"].getString() != U"double")
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			if (not radius.contains(U"value") || not radius[U"value"].isNumber())
			{
				Editor::ShowError(U"CircleObject のパースに失敗しました。");
				return result;
			}

			result.radius = radius[U"value"].get<double>();
		}

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
		}

		Circle{ circleObject.center,circleObject.radius }.draw();

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
