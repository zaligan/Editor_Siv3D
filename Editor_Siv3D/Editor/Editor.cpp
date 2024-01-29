# include "Editor.hpp"
# include "NotificationAddon.hpp"

bool Editor::init()
{
	// アドオンを登録する
	Addon::Register<NotificationAddon>(U"NotificationAddon");

	NotificationAddon::SetLifeTime(5.0);

	NotificationAddon::SetStyle({ .width = 900 });

	return true;
}

bool Editor::prepareConfigDirectory()
{
	if (not FileSystem::IsDirectory(m_configDirectory))
	{
		if (not FileSystem::CreateDirectories(m_configDirectory))
		{
			ShowError(U"configフォルダ`{}`の作成に失敗しました。同名のファイルが既に存在します。"_fmt(m_configDirectory));
			return false;
		}

		ShowSuccess(U"configフォルダ`{}`を作成しました。"_fmt(m_configDirectory));
	}
	else
	{
		ShowSuccess(U"config フォルダ`{}`が見つかりました"_fmt(m_configDirectory));
	}

	m_configDirectoryWatcher = DirectoryWatcher{ m_configDirectory };

	if (not m_configDirectoryWatcher)
	{
		ShowError(U"configディレクトリ`{}`の監視を開始できませんでした。"_fmt(m_configDirectory));
		return false;
	}

	ShowSuccess(U"configディレクトリ`{}`の監視を開始しました。"_fmt(m_configDirectory));

	return true;
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
