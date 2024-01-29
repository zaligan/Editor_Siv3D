﻿# pragma once
# include <Siv3D.hpp>

class Editor
{
public:
	/// @brief エディタを初期化します
	/// @return メイン関数の先頭で１度だけ呼び出してください
	[[nodiscard]]
	bool init();

	/// @brief 通知（詳細）を出力します。
	/// @param text 通知内容
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

};

