#include "stdafx.h"
#include "MainMenu.h"
#include <SFML/Window/Event.hpp>
#include "Launcher.h"
#include <cegui/ScriptModules/Lua/ScriptModule.h>

int tolua_Launcher_open (lua_State* tolua_S);

MainMenu::MainMenu() {
  initializeMaps();

  renderer = &CEGUI::OpenGLRenderer::bootstrapSystem();
  CEGUI::LuaScriptModule *scriptModule = &CEGUI::LuaScriptModule::create();

  lua_State *luaState = scriptModule->getLuaState();
  tolua_Launcher_open(luaState);

  CEGUI::System::getSingleton().setScriptingModule(scriptModule);

  CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
      (CEGUI::System::getSingleton().getResourceProvider());
  rp->setResourceGroupDirectory("schemes", "./schemes/");
  rp->setResourceGroupDirectory("imagesets", "./imagesets/");
  rp->setResourceGroupDirectory("fonts", "./fonts/");
  rp->setResourceGroupDirectory("layouts", "./layouts/");
  rp->setResourceGroupDirectory("looknfeels", "./looknfeel/");
  rp->setResourceGroupDirectory("lua_scripts", "./lua_scripts/");

  CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
  CEGUI::Font::setDefaultResourceGroup("fonts");
  CEGUI::Scheme::setDefaultResourceGroup("schemes");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
  CEGUI::WindowManager::setDefaultResourceGroup("layouts");
  CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
  CEGUI::Scheme::setDefaultResourceGroup("schemes");

  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::SchemeManager::getSingleton().createFromFile("Generic.scheme");
  CEGUI::SchemeManager::getSingleton().createFromFile("Paintball.scheme");

  CEGUI::Window *root = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("Paintball.layout");
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(root);

  CEGUI::System::getSingleton().executeScriptFile("Paintball.lua");
}

void MainMenu::Draw() { 
  CEGUI::System::getSingleton().renderAllGUIContexts();
}

void MainMenu::OnConnectionFailed() {
  CEGUI::System::getSingleton().executeScriptGlobal("onConnectionFailed");
}

void MainMenu::OnGameEnded(GameStatus status) {
  std::string message;
  switch (status) {
    case kFinished:
      message = "Game ended.";
      break;
    case kDisconnected:
      message = "Opponent has disconnected.";
      break;
    case kDesynced:
      message = "Desync detected";
      break;
    default:
      assert(false);
  }
  CEGUI::System::getSingleton().executeScriptString("onGameEnded(\"" + message + "\")");
}

void MainMenu::HandleEvent(sf::Event &Event) {
  switch (Event.type) {
    case sf::Event::TextEntered:
      defaultContext().injectChar(Event.text.unicode);
      break;
    case sf::Event::KeyPressed:
      defaultContext().injectKeyDown(toCEGUIKey(Event.key.code));
      break;
    case sf::Event::KeyReleased:
      defaultContext().injectKeyUp(toCEGUIKey(Event.key.code));
      break;
    case sf::Event::MouseMoved:
      defaultContext().injectMousePosition(static_cast<float>(Event.mouseMove.x),
          static_cast<float>(Event.mouseMove.y));
      break;
    case sf::Event::MouseButtonPressed:
      defaultContext().injectMouseButtonDown(toCEGUIMouseButton(Event.mouseButton.button));
      break;
    case sf::Event::MouseButtonReleased:
      defaultContext().injectMouseButtonUp(toCEGUIMouseButton(Event.mouseButton.button));
      break;
    case sf::Event::MouseWheelMoved:
      defaultContext().injectMouseWheelChange(static_cast<float>(Event.mouseWheel.delta));
      break;
    case sf::Event::Resized:
      CEGUI::System::getSingleton().notifyDisplaySizeChanged(
          CEGUI::Sizef((float)Event.size.width, (float)Event.size.height));
  }
}

CEGUI::Key::Scan MainMenu::toCEGUIKey(sf::Keyboard::Key Code)
{
  if (mKeyMap.find(Code) == mKeyMap.end())
    return (CEGUI::Key::Scan)0;

  return mKeyMap[Code];
}

CEGUI::MouseButton MainMenu::toCEGUIMouseButton(sf::Mouse::Button Button)
{
  if (mMouseButtonMap.find(Button) == mMouseButtonMap.end())
    return (CEGUI::MouseButton)0;

  return mMouseButtonMap[Button];
}

//Auto-generated (phew...), edited by hand
void MainMenu::initializeMaps()
{
  mKeyMap[sf::Keyboard::Key::Escape]          = CEGUI::Key::Escape       ;
  mKeyMap[sf::Keyboard::Key::Num1]            = CEGUI::Key::One          ;
  mKeyMap[sf::Keyboard::Key::Num2]            = CEGUI::Key::Two          ;
  mKeyMap[sf::Keyboard::Key::Num3]            = CEGUI::Key::Three        ;
  mKeyMap[sf::Keyboard::Key::Num4]            = CEGUI::Key::Four         ;
  mKeyMap[sf::Keyboard::Key::Num5]            = CEGUI::Key::Five         ;
  mKeyMap[sf::Keyboard::Key::Num6]            = CEGUI::Key::Six          ;
  mKeyMap[sf::Keyboard::Key::Num7]            = CEGUI::Key::Seven        ;
  mKeyMap[sf::Keyboard::Key::Num8]            = CEGUI::Key::Eight        ;
  mKeyMap[sf::Keyboard::Key::Num9]            = CEGUI::Key::Nine         ;
  mKeyMap[sf::Keyboard::Key::Num0]            = CEGUI::Key::Zero         ;
  mKeyMap[sf::Keyboard::Key::Dash]            = CEGUI::Key::Minus        ;
  mKeyMap[sf::Keyboard::Key::Equal]           = CEGUI::Key::Equals       ;
  mKeyMap[sf::Keyboard::Key::BackSpace]       = CEGUI::Key::Backspace    ;
  mKeyMap[sf::Keyboard::Key::Tab]             = CEGUI::Key::Tab          ;
  mKeyMap[sf::Keyboard::Key::Q]               = CEGUI::Key::Q            ;
  mKeyMap[sf::Keyboard::Key::W]               = CEGUI::Key::W            ;
  mKeyMap[sf::Keyboard::Key::E]               = CEGUI::Key::E            ;
  mKeyMap[sf::Keyboard::Key::R]               = CEGUI::Key::R            ;
  mKeyMap[sf::Keyboard::Key::T]               = CEGUI::Key::T            ;
  mKeyMap[sf::Keyboard::Key::Y]               = CEGUI::Key::Y            ;
  mKeyMap[sf::Keyboard::Key::U]               = CEGUI::Key::U            ;
  mKeyMap[sf::Keyboard::Key::I]               = CEGUI::Key::I            ;
  mKeyMap[sf::Keyboard::Key::O]               = CEGUI::Key::O            ;
  mKeyMap[sf::Keyboard::Key::P]               = CEGUI::Key::P            ;
  mKeyMap[sf::Keyboard::Key::LBracket]        = CEGUI::Key::LeftBracket  ;
  mKeyMap[sf::Keyboard::Key::RBracket]        = CEGUI::Key::RightBracket ;
  mKeyMap[sf::Keyboard::Key::Return]          = CEGUI::Key::Return       ;
  mKeyMap[sf::Keyboard::Key::LControl]        = CEGUI::Key::LeftControl  ;
  mKeyMap[sf::Keyboard::Key::A]               = CEGUI::Key::A            ;
  mKeyMap[sf::Keyboard::Key::S]               = CEGUI::Key::S            ;
  mKeyMap[sf::Keyboard::Key::D]               = CEGUI::Key::D            ;
  mKeyMap[sf::Keyboard::Key::F]               = CEGUI::Key::F            ;
  mKeyMap[sf::Keyboard::Key::G]               = CEGUI::Key::G            ;
  mKeyMap[sf::Keyboard::Key::H]               = CEGUI::Key::H            ;
  mKeyMap[sf::Keyboard::Key::J]               = CEGUI::Key::J            ;
  mKeyMap[sf::Keyboard::Key::K]               = CEGUI::Key::K            ;
  mKeyMap[sf::Keyboard::Key::L]               = CEGUI::Key::L            ;
  mKeyMap[sf::Keyboard::Key::SemiColon]       = CEGUI::Key::Semicolon    ;
  mKeyMap[sf::Keyboard::Key::LShift]          = CEGUI::Key::LeftShift    ;
  mKeyMap[sf::Keyboard::Key::BackSlash]       = CEGUI::Key::Backslash    ;
  mKeyMap[sf::Keyboard::Key::Z]               = CEGUI::Key::Z            ;
  mKeyMap[sf::Keyboard::Key::X]               = CEGUI::Key::X            ;
  mKeyMap[sf::Keyboard::Key::C]               = CEGUI::Key::C            ;
  mKeyMap[sf::Keyboard::Key::V]               = CEGUI::Key::V            ;
  mKeyMap[sf::Keyboard::Key::B]               = CEGUI::Key::B            ;
  mKeyMap[sf::Keyboard::Key::N]               = CEGUI::Key::N            ;
  mKeyMap[sf::Keyboard::Key::M]               = CEGUI::Key::M            ;
  mKeyMap[sf::Keyboard::Key::Comma]           = CEGUI::Key::Comma        ;
  mKeyMap[sf::Keyboard::Key::Period]          = CEGUI::Key::Period       ;
  mKeyMap[sf::Keyboard::Key::Slash]           = CEGUI::Key::Slash        ;
  mKeyMap[sf::Keyboard::Key::RShift]          = CEGUI::Key::RightShift   ;
  mKeyMap[sf::Keyboard::Key::Multiply]        = CEGUI::Key::Multiply     ;
  mKeyMap[sf::Keyboard::Key::LAlt]            = CEGUI::Key::LeftAlt      ;
  mKeyMap[sf::Keyboard::Key::Space]           = CEGUI::Key::Space        ;
  mKeyMap[sf::Keyboard::Key::F1]              = CEGUI::Key::F1           ;
  mKeyMap[sf::Keyboard::Key::F2]              = CEGUI::Key::F2           ;
  mKeyMap[sf::Keyboard::Key::F3]              = CEGUI::Key::F3           ;
  mKeyMap[sf::Keyboard::Key::F4]              = CEGUI::Key::F4           ;
  mKeyMap[sf::Keyboard::Key::F5]              = CEGUI::Key::F5           ;
  mKeyMap[sf::Keyboard::Key::F6]              = CEGUI::Key::F6           ;
  mKeyMap[sf::Keyboard::Key::F7]              = CEGUI::Key::F7           ;
  mKeyMap[sf::Keyboard::Key::F8]              = CEGUI::Key::F8           ;
  mKeyMap[sf::Keyboard::Key::F9]              = CEGUI::Key::F9           ;
  mKeyMap[sf::Keyboard::Key::F10]             = CEGUI::Key::F10          ;
  mKeyMap[sf::Keyboard::Key::Numpad7]         = CEGUI::Key::Numpad7      ;
  mKeyMap[sf::Keyboard::Key::Numpad8]         = CEGUI::Key::Numpad8      ;
  mKeyMap[sf::Keyboard::Key::Numpad9]         = CEGUI::Key::Numpad9      ;
  mKeyMap[sf::Keyboard::Key::Subtract]        = CEGUI::Key::Subtract     ;
  mKeyMap[sf::Keyboard::Key::Numpad4]         = CEGUI::Key::Numpad4      ;
  mKeyMap[sf::Keyboard::Key::Numpad5]         = CEGUI::Key::Numpad5      ;
  mKeyMap[sf::Keyboard::Key::Numpad6]         = CEGUI::Key::Numpad6      ;
  mKeyMap[sf::Keyboard::Key::Add]             = CEGUI::Key::Add          ;
  mKeyMap[sf::Keyboard::Key::Numpad1]         = CEGUI::Key::Numpad1      ;
  mKeyMap[sf::Keyboard::Key::Numpad2]         = CEGUI::Key::Numpad2      ;
  mKeyMap[sf::Keyboard::Key::Numpad3]         = CEGUI::Key::Numpad3      ;
  mKeyMap[sf::Keyboard::Key::Numpad0]         = CEGUI::Key::Numpad0      ;
  mKeyMap[sf::Keyboard::Key::F11]             = CEGUI::Key::F11          ;
  mKeyMap[sf::Keyboard::Key::F12]             = CEGUI::Key::F12          ;
  mKeyMap[sf::Keyboard::Key::F13]             = CEGUI::Key::F13          ;
  mKeyMap[sf::Keyboard::Key::F14]             = CEGUI::Key::F14          ;
  mKeyMap[sf::Keyboard::Key::F15]             = CEGUI::Key::F15          ;
  mKeyMap[sf::Keyboard::Key::RControl]        = CEGUI::Key::RightControl ;
  mKeyMap[sf::Keyboard::Key::Divide]          = CEGUI::Key::Divide       ;
  mKeyMap[sf::Keyboard::Key::RAlt]            = CEGUI::Key::RightAlt     ;
  mKeyMap[sf::Keyboard::Key::Pause]           = CEGUI::Key::Pause        ;
  mKeyMap[sf::Keyboard::Key::Home]            = CEGUI::Key::Home         ;
  mKeyMap[sf::Keyboard::Key::Up]              = CEGUI::Key::ArrowUp      ;
  mKeyMap[sf::Keyboard::Key::PageUp]          = CEGUI::Key::PageUp       ;
  mKeyMap[sf::Keyboard::Key::Left]            = CEGUI::Key::ArrowLeft    ;
  mKeyMap[sf::Keyboard::Key::Right]           = CEGUI::Key::ArrowRight   ;
  mKeyMap[sf::Keyboard::Key::End]             = CEGUI::Key::End          ;
  mKeyMap[sf::Keyboard::Key::Down]            = CEGUI::Key::ArrowDown    ;
  mKeyMap[sf::Keyboard::Key::PageDown]        = CEGUI::Key::PageDown     ;
  mKeyMap[sf::Keyboard::Key::Insert]          = CEGUI::Key::Insert       ;
  mKeyMap[sf::Keyboard::Key::Delete]          = CEGUI::Key::Delete       ;

  mMouseButtonMap[sf::Mouse::Left]		= CEGUI::LeftButton;
  mMouseButtonMap[sf::Mouse::Middle]		= CEGUI::MiddleButton;
  mMouseButtonMap[sf::Mouse::Right]		= CEGUI::RightButton;
  mMouseButtonMap[sf::Mouse::XButton1]	= CEGUI::X1Button;
  mMouseButtonMap[sf::Mouse::XButton2]	= CEGUI::X2Button;
}