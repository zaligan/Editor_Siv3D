# include "Editor.hpp"
# include "NotificationAddon.hpp"

namespace
{
	[[nodiscard]]
	static String ToString(const FileAction fileAction)
	{
		switch (fileAction)
		{
		case FileAction::Added:
			return U"Added";
		case FileAction::Modified:
			return U"Modified";
		case FileAction::Removed:
			return U"Removed";
		default:
			return U"Unknown";
		}
	}
}

bool Editor::init()
{
	// アドオンを登録する
	Addon::Register<NotificationAddon>(U"NotificationAddon");

	//画面上に通知を表示しておく時間(秒)
	NotificationAddon::SetLifeTime(10.0);

	//通知の横幅を設定する
	NotificationAddon::SetStyle({ .width = 900 });

	return true;
}

bool Editor::prepareConfigDirectory()
{
	//config ディレクトリが見つからない場合作成する
	if (not FileSystem::IsDirectory(m_configDirectory))
	{
		if (not FileSystem::CreateDirectories(m_configDirectory))
		{
			ShowError(U"configディレクトリ`{}`の作成に失敗しました。同名のファイルが既に存在します。"_fmt(m_configDirectory));
			return false;
		}

		ShowSuccess(U"configディレクトリ`{}`を作成しました。"_fmt(m_configDirectory));
	}
	else
	{
		ShowSuccess(U"config ディレクトリ`{}`が見つかりました"_fmt(m_configDirectory));
	}

	//config ディレクトリを監視する
	m_configDirectoryWatcher = DirectoryWatcher{ m_configDirectory };

	if (not m_configDirectoryWatcher)
	{
		ShowError(U"configディレクトリ`{}`の監視を開始できませんでした。"_fmt(m_configDirectory));
		return false;
	}

	ShowSuccess(U"configディレクトリ`{}`の監視を開始しました。"_fmt(m_configDirectory));

	return true;
}

void Editor::update()
{
	// 絶対パスと、アクションの内容を取得する
	for (const auto& [path, fileAction] : m_configDirectoryWatcher.retrieveChanges())
	{
		//監視対象でない拡張子の場合無視する
		const String extension = FileSystem::Extension(path);
		if (not m_configDirectoryAllowExtensions.contains(extension))
		{
			continue;
		}

		ShowVerbose(U"File {}:`{}`"_fmt(ToString(fileAction), path));
	}
}

void Editor::ShowVerbose([[maybe_unused]]const StringView text)
{
# if SIV3D_BUILD(DEBUG)
	NotificationAddon::Show(text, NotificationAddon::Type::Normal);
# endif
}

void Editor::ShowInfo(const StringView text)
{
	NotificationAddon::Show(text, NotificationAddon::Type::Information);
}

void Editor::ShowSuccess(const StringView text)
{
	NotificationAddon::Show(text, NotificationAddon::Type::Success);
}

void Editor::ShowWarning(const StringView text)
{
	NotificationAddon::Show(text, NotificationAddon::Type::Warning);
}

void Editor::ShowError(const StringView text)
{
	NotificationAddon::Show(text, NotificationAddon::Type::Failure);
}
