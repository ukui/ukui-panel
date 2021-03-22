#include "panel_commission.h"

PanelCommission::PanelCommission()
{
//    system("/usr/share/ukui/ukui-panel/ukui-panel-reset.sh");
}

PanelCommission::~PanelCommission()
{
}

void PanelCommission::panelConfigFileReset(bool reset)
{
    if(reset)
        system("/usr/share/ukui/ukui-panel/ukui-panel-reset.sh");
}

void PanelCommission::panelConfigFileValueInit(bool set)
{
    if(set)
        system("/usr/share/ukui/ukui-panel/panel-commission.sh");
}
