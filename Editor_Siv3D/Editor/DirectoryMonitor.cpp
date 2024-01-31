# include "DirectoryMonitor.hpp"
# include "Editor.hpp"

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

bool DirectoryMonitor::init(FilePathView directory, const Array<String>& allowExtensions, int32 cooldownTimeMillisec)
{
	m_directory = directory;
	m_allowExtensions = allowExtensions;
	m_cooldownTimeMillisec = cooldownTimeMillisec;
	m_directoryWatcher = DirectoryWatcher{ m_directory };

	//ディレクトリが見つからない場合作成する
	if (not FileSystem::IsDirectory(m_directory))
	{
		if (not FileSystem::CreateDirectories(m_directory))
		{
			Editor::ShowError(U"ディレクトリ`{}`の作成に失敗しました。同名のファイルが既に存在します。"_fmt(m_directory));
			return false;
		}

		Editor::ShowSuccess(U"ディレクトリ`{}`を作成しました。"_fmt(m_directory));
	}
	else
	{
		Editor::ShowSuccess(U"ディレクトリ`{}`が見つかりました"_fmt(m_directory));
	}

	if (not m_directoryWatcher)
	{
		Editor::ShowError(U"ディレクトリ`{}`の監視を開始できませんでした。"_fmt(m_directory));
		return false;
	}

	Editor::ShowSuccess(U"ディレクトリ`{}`の監視を開始しました。"_fmt(m_directory));

	//既存のファイルをバッファに追加
	for (const auto& path : FileSystem::DirectoryContents(m_directory))
	{
		if (const String extension = FileSystem::Extension(path);
			(not m_allowExtensions.contains(extension)))
		{
			//監視対象の拡張子でない場合は無視する。
			continue;
		}

		m_changeFileBuffer.emplace(path, 0);
	}

	return true;
}

void DirectoryMonitor::update()
{
	uint64 currentTimeMillisec = Time::GetMillisec();
	// 絶対パスと、アクションの内容を取得する
	for (const auto& [path, fileAction] : m_directoryWatcher.retrieveChanges())
	{
		//監視対象でない拡張子の場合無視する
		const String extension = FileSystem::Extension(path);
		if (not m_allowExtensions.contains(extension))
		{
			continue;
		}

		Editor::ShowVerbose(U"File {}:`{}`"_fmt(ToString(fileAction), path));


		//バッファーに無いファイルの変更の場合バッファーにpathと時間を追加
		if (auto it = m_changeFileBuffer.find(path); (it != m_changeFileBuffer.end()))
		{
			it->second = currentTimeMillisec;
		}
		else
		{
			m_changeFileBuffer.emplace(path, currentTimeMillisec);
		}
	}
}

Array<FilePath> DirectoryMonitor::retrieveChangedFiles()
{
	const uint64 currentTimeMillisec = Time::GetMillisec();
	Array<FilePath> changedFiles;
	//最終更新から一定時間変更のないファイルを読み込む
	for (auto it = m_changeFileBuffer.begin(); it != m_changeFileBuffer.end();)
	{
		if (currentTimeMillisec < it->second + m_cooldownTimeMillisec)
		{
			
			++it;
			continue;
		}

		changedFiles << it->first;
		it = m_changeFileBuffer.erase(it);
	}

	return changedFiles;
}
