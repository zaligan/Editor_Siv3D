# include "Editor.hpp"
# include "NotificationAddon.hpp"

bool Editor::init()
{
	// アドオンを登録する
	Addon::Register<NotificationAddon>(U"NotificationAddon");

	NotificationAddon::SetLifeTime(5.0);

	NotificationAddon::SetStyle({ .width = 900 });

	return (true);
}

void Editor::ShowVerbose(const StringView text)
{
	NotificationAddon::Show(text, NotificationAddon::Type::Normal);
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
