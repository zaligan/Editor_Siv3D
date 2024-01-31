# pragma once
# include <Siv3D.hpp>

class DirectoryMonitor
{
public:
	DirectoryMonitor() = default;

	bool init(FilePathView directory, const Array<String>& allowExtensions, int32 cooldownTimeMillisec = 100);

	void update();

	Array<FilePath> retrieveChangedFiles();

private:

	/// @brief ディレクトリのパス
	FilePath m_directory;

	/// @brief ディレクトリの監視オブジェクト
	DirectoryWatcher m_directoryWatcher;

	/// @brief ディレクトリで監視する拡張子
	Array<String> m_allowExtensions;

	/// @brief バッファ内のファイルが最終更新からこの時間（ミリ秒）経過後に読み込まれる閾値
	int32 m_cooldownTimeMillisec = 100;

	/// @brief 変更されたファイルのパスと更新時間（ミリ秒）
	HashTable<FilePath, uint64> m_changeFileBuffer;
};
