# pragma once
# include <Siv3D.hpp>

class Editor
{
public:
	/// @brief エディタを初期化します
	/// @return メイン関数の先頭で１度だけ呼び出してください
	[[nodiscard]]
	bool init();

	static void ShowVerbose(StringView text);

	static void ShowInfo(StringView text);

	static void ShowSuccess(StringView text);

	static void ShowWarning(StringView text);

	static void ShowError(StringView text);
private:

};

