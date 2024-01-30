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

		//バッファーに無いファイルの変更の場合バッファーにpathと時間を追加
		if (not m_changeFileBuffer.contains(path))
		{
			m_changeFileBuffer.emplace(path,Time::GetMillisec());
			continue;
		}

		//既にバッファーにあるファイルの変更の場合、変更時間を更新する	
		for (auto it = m_changeFileBuffer.begin(); it != m_changeFileBuffer.end(); ++it)
		{
			if (it->first != path)
			{
				continue;
			}

			it->second = Time::GetMillisec();
		}

	}

	//最終更新から一定時間変更のないファイルを読み込む
	for (auto it = m_changeFileBuffer.begin(); it != m_changeFileBuffer.end();)
	{
		if (it->second + m_bufferReadDelay > Time::GetMillisec())
		{
			++it;
			continue;
		}

		ShowVerbose(U"File `{}`が更新されました。"_fmt(it->first));
		
		it = m_changeFileBuffer.erase(it);
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
