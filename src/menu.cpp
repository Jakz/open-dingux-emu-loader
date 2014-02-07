#include "menu.h"

#include "menu_view.h"

using namespace std;
using namespace gcw;

void SubMenuEntry::action(MenuView *view, GCWKey key)
{
  if (key == GCW_KEY_RIGHT)
    view->enterSubmenu(this);
}


void BoolMenuEntry::action(MenuView *view, GCWKey key)
{
  if (key == GCW_KEY_RIGHT || key == GCW_KEY_LEFT)
  {
    bool newValue = !setting->getValue();
    
    caption = setting->getName() + " " + (newValue ? "true" : "false");
    setting->setValue(newValue);
  }
}



ConsoleMenu::ConsoleMenu(string caption, vector<ConsoleSpec*>* consoles) : Menu("Browse by System"), consoles(consoles)
{
  
}