#include "path_view.h"

#include "utils.h"
#include "gfx.h"

using namespace std;
using namespace gcw;

u32 PathView::PathList::current() { return view->index; }
u32 PathView::PathList::count() { return static_cast<u32>(view->folders.size()); }
void PathView::PathList::set(u32 i) { view->index = i; }
string& PathView::PathList::selected() { return view->folders[view->index]; }
string& PathView::PathList::get(u32 i) { return view->folders[offset+view->index]; }

PathView::PathView(Manager* manager) : View(manager), index(0), list(this) { }

void PathView::init(string title, Path *path)
{
  this->title = title;
  this->path = path;
  this->folders = path->subfolders();
}

void PathView::render()
{
  gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y, false, Font::bigFont, title.c_str());
  
  const int MAX_LENGTH = 30;
  string path = this->path->value();

  if (path.length() < MAX_LENGTH)
    gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y+10, false, Font::bigFont, path.c_str());
  else
    gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y+10, false, Font::bigFont, (path.substr(0, MAX_LENGTH)+"...").c_str());
  
  u32 count = list.getDisplayedAmount();
  for (int i = 0; i < count; ++i)
  {
    string &folder = folders[i+list.minOffset()];
    
    const int MAX_LENGTH = 30;
    
    if (folder.length() < MAX_LENGTH)
      gfx->print(View::MENU_OFFSET.x, View::MENU_OFFSET.y+14*i, false, Font::bigFont, folder.c_str());
    else
      gfx->print(View::MENU_OFFSET.x, View::MENU_OFFSET.y+14*i, false, Font::bigFont, (folder.substr(0, MAX_LENGTH)+"...").c_str());
  }
  
  gfx->print(View::MENU_OFFSET.x-10,View::MENU_OFFSET.y+list.relativeIndex(list.current())*14, false, Font::bigFont, ">");
  
}

void PathView::handleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
        
        // manage normal keys
      case SDL_KEYDOWN:
      {
        GCWKey key = static_cast<GCWKey>(event.key.keysym.sym);
        
        switch (key)
        {
          case GCW_KEY_DOWN: list.down(); break;
          case GCW_KEY_UP: list.up(); break;
            
          case GCW_KEY_L: list.prevPage(); break;
          case GCW_KEY_R: list.nextPage(); break;
            
          case MENU_BACK_BUTTON:
            
            break;
            
          //case GCW_KEY_RIGHT:
          //case GCW_KEY_LEFT:
          case MENU_ACTION_BUTTON:
            if (list.selected() == ".." && !path->isRoot())
            {
              path->removeLast();
              folders = path->subfolders();
              list.reset();
            }
            else
            {
              path->append(list.selected());
              folders = path->subfolders();
              list.reset();
            }
            
            break;
            
          default: break;
        }
      }
    }
  }
}