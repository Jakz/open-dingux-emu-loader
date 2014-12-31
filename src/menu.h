#ifndef _MENU_H_
#define _MENU_H_

#include "defines.h"

#include "gfx.h"
#include "settings.h"
#include "rom_collection.h"
#include "persistence.h"
#include "loader.h"

#include <string>
#include <vector>
#include <memory>

namespace gcw {
  
class Menu;
class MenuView;

class MenuEntry
{
  public:
    MenuEntry() { }
    virtual ~MenuEntry() { }
    virtual const std::string& name() = 0;
  
    virtual void action(Manager *manager, GCWKey key) { }
    virtual void render(Gfx* gfx, int x, int y);
};
  
class StandardMenuEntry : public MenuEntry
{
  protected:
    const std::string caption;
    
  public:
    StandardMenuEntry(std::string caption) : caption(caption) { }
    virtual const std::string& name() { return caption; }
    
    virtual void action(Manager *manager, GCWKey key) { }
};
  
class LambdaMenuEntry : public StandardMenuEntry
{
  protected:
    std::function<void (Manager*)> lambda;
  
  public:
    LambdaMenuEntry(std::string caption, std::function<void (Manager*)> lambda) : StandardMenuEntry(caption), lambda(lambda) { }

    virtual void action(Manager *manager, GCWKey key) { if (key == GCW_KEY_B) lambda(manager); }
};

class SubMenuEntry : public StandardMenuEntry
{
  protected:
    Menu* const menu;
 
  public:
    SubMenuEntry(std::string caption, Menu *menu) : StandardMenuEntry(caption+" >"), menu(menu) { }
  
    Menu *subMenu() { return menu; }
  
    virtual void action(Manager *manager, GCWKey key);
};
  
class BoolMenuEntry : public StandardMenuEntry
{
  private:
    BoolSetting* const setting;
  
  public:
    BoolMenuEntry(BoolSetting *setting) : StandardMenuEntry(setting->getName()), setting(setting) { }

    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
class EnumMenuEntry : public StandardMenuEntry
{
  private:
    EnumSetting* const setting;
  
  public:
    EnumMenuEntry(EnumSetting* setting) : StandardMenuEntry(setting->getName()), setting(setting) { }
  
    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
class PathSettingMenuEntry : public StandardMenuEntry
{
  private:
    PathSetting* const setting;
    const std::string pathViewTitle;
  public:
    PathSettingMenuEntry(PathSetting *setting, const std::string& pathViewTitle) : StandardMenuEntry(setting->getName()), setting(setting), pathViewTitle(pathViewTitle) { }
  
    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
class PathMenuEntry : public MenuEntry
{
  private:
    Path path;
  public:
    PathMenuEntry(const Path& path) : path(path) { }
    
    virtual const std::string& name() { return path.value(); }
    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
  
class SystemMenuEntry : public SubMenuEntry
{
private:
  SystemSpec* const system;
  SDL_Surface* const icon;
  
public:
  SystemMenuEntry(SystemSpec *system, Menu *menu);
  virtual void render(Gfx* gfx, int x, int y);
};

class RomMenuEntry : public MenuEntry
{
  private:
    RomEntry* const rom;
  
  public:
    RomMenuEntry(RomEntry *rom) : rom(rom) { }
    virtual const std::string& name() { return rom->name; }
    virtual void render(Gfx* gfx, int x, int y);
};



class Menu
{
  protected:
    std::string const caption;
  
  public:
    Menu(std::string caption) : caption(caption) { }
  
    virtual size_t count() const = 0;
    virtual MenuEntry* entryAt(u32 index) = 0;
    virtual void render(Gfx* gfx, int offset, int size, int tx, int ty, int x, int y);

  
    const std::string& title() { return caption; }
};
  
class StandardMenu : public Menu
{
  protected:
    std::vector<std::unique_ptr<MenuEntry>> entries;
  
  public:
    StandardMenu(std::string caption) : Menu(caption) { }
  
    size_t count() const { return entries.size(); }
    void addEntry(MenuEntry *entry) { entries.push_back(std::unique_ptr<MenuEntry>(entry)); }
    MenuEntry* entryAt(u32 index) { return entries[index].get(); }

};
  
class RomPathsMenu : public StandardMenu
{
  private:
    Persistence* const persistence;
  
  public:
    RomPathsMenu(std::string caption, Persistence *persistence) : StandardMenu(caption), persistence(persistence) { }
  
    void build();
  
};
  

class RomsMenu : public StandardMenu
{
public:
  RomsMenu(std::string caption, RomIteratorRange roms) : StandardMenu(caption)
  {
    for (RomIterator it = roms.first; it != roms.second; ++it)
    {
      RomEntry *rom = &it->second;
      entries.push_back(std::unique_ptr<MenuEntry>(new RomMenuEntry(rom)));
    }
  }
};
  
  
class SystemsMenu : public StandardMenu
{
  private:
  
  public:
    SystemsMenu(std::string caption, RomCollection *collection) : StandardMenu(caption)
    {
      std::vector<SystemSpec>* systems = collection->getSystems();
      std::vector<SystemSpec>::iterator it;
      
      for (it = systems->begin(); it != systems->end(); ++it)
      {
        SystemSpec *system = &(*it);
        RomIteratorRange roms = collection->getRomsForSystem(system);
        
        if (roms.first != roms.second)
          entries.push_back(std::unique_ptr<MenuEntry>(new SystemMenuEntry(system, new RomsMenu(system->name, roms))));
      }

    }
};
  
  class CoreMenuEntry : public StandardMenuEntry
  {
    private:
      const std::unique_ptr<CoreHandle>& core;
    public:
      CoreMenuEntry(const std::unique_ptr<CoreHandle>& core) : StandardMenuEntry(core->info.title()), core(core) { }
  };
  
  class CoresMenu : public StandardMenu
  {
  private:
    
  public:
    CoresMenu(Loader* loader) : StandardMenu("Cores")
    {
      auto& cores = loader->getCores();
      
      for (const std::unique_ptr<CoreHandle>& core : cores)
        entries.push_back(std::unique_ptr<MenuEntry>(new CoreMenuEntry(core)));
    }
  };
  

  
}

#endif
