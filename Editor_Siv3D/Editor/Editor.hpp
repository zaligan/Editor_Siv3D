# pragma once
# include <Siv3D.hpp>
# include "DirectoryMonitor.hpp"

class Editor
{
public:
	/// @brief エディタを初期化します
	/// @return メイン関数の先頭で１度だけ呼び出してください
	[[nodiscard]]
	bool init();

	/// @brief configディレクトリを準備します。
	/// @return 準備に成功した場合 true,それ以外の場合はfalse
	[[nodiscard]]
	bool prepareConfigDirectory();

	/// @brief エディタの状態を更新します。
	void update();

	[[nodiscard]]
	Array<FilePath> retrieveChangedConfigFiles();

	/// @brief 通知（詳細）を出力します。
	/// @param text 通知内容
	/// @remark Relese ビルドでは通知は出力されません。
	static void ShowVerbose(StringView text);

	/// @brief 通知（情報）を出力します。
	/// @param text 通知内容
	static void ShowInfo(StringView text);

	/// @brief 通知（成功）を出力します。
	/// @param text 通知内容
	static void ShowSuccess(StringView text);

	/// @brief 通知（警告）を出力します。
	/// @param text 通知内容
	static void ShowWarning(StringView text);

	/// @brief 通知（失敗）を出力します。
	/// @param text 通知内容
	static void ShowError(StringView text);

private:

	DirectoryMonitor m_configDirectoryMonitor;


};

