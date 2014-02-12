#pragma once
#include "wx/wx.h"

class HostGameForm : public wxWindow {
  public:
    HostGameForm(wxWindow *parent);

  private:
    wxBoxSizer *sizer;
    wxTextCtrl *portBox;
};