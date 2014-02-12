#include "stdafx.h"
#include "Launcher.h"
#include "LauncherFrame.h"

IMPLEMENT_APP(Launcher)

bool Launcher::OnInit() {
  wxInitAllImageHandlers();
  LauncherFrame *frame = new LauncherFrame(_("Game"), wxPoint(50, 50),
    wxSize(1280, 720));
  frame->Show(true);
  SetTopWindow(frame);
  return true;
}