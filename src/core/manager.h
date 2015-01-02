#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "loader.h"
#include "../ui/gfx.h"
#include "../common/utils.h"
#include "../data/rom_collection.h"
#include "../data/persistence.h"

#include "../views/core_view.h"
#include "../views/menu_view.h"
#include "../views/path_view.h"

namespace gcw {
  
class Manager
{
  private:
    CoreInterface *core;
    Loader loader;
    Persistence persistence;
    RomCollection collection;
    Gfx gfx;
    Timer timer;
  
    CoreView coreView;
    MenuView menuView;
    PathView pathView;
    View *currentView;
  
    bool running;

  public:
    Manager() : core(nullptr), loader(this), collection(this), gfx(Gfx()), coreView(this), menuView(this), pathView(this), currentView(nullptr), running(true) { }
    void scan() { loader.scan(); }
    void init();
    void run();
  
    MenuView *getMenuView() { return &menuView; }
    PathView *getPathView() { return &pathView; }
  
    Loader *getLoader() { return &loader; }
    Persistence *getPersistence() { return &persistence; }
    RomCollection *getRomCollection() { return &collection; }
    Gfx *getGfx() { return &gfx; }
    Timer *getTimer() { return &timer; }
  
    void switchView(ViewType type)
    {
      switch (type)
      {
        case VIEW_MENU: currentView = &menuView; break;
        case VIEW_CORE: currentView = &coreView; break;
        case VIEW_PATH: currentView = &pathView; break;
      }
    }
  
    void exit() { running = false; }
  
  
  
};

}

#endif