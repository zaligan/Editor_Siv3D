# include "Editor.hpp"
# include "NotificationAddon.hpp"

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
	if (not m_configDirectoryMonitor.init(U"config/", { U"json",U"ini",U"txt"}))
	{
		return false;
	}
	return true;
}

void Editor::update()
{
	m_configDirectoryMonitor.update();
}

Array<FilePath> Editor::retrieveChangedConfigFiles()
{
	return m_configDirectoryMonitor.retrieveChangedFiles();
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
