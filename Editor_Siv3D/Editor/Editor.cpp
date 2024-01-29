# include "Editor.hpp"

bool Editor::init()
{
	// アドオンを登録する
	Addon::Register<NotificationAddon>(U"NotificationAddon");

	NotificationAddon::SetLifeTime(5.0);

	NotificationAddon::SetStyle({ .width = 900 });

	return (true);
}
