#include "storagearrow.h"
#include "../panel/customstyle.h"
TrayButton::TrayButton(QWidget* parent):
    QToolButton(parent)
{
    setStyle(new CustomStyle());
    setIcon(QIcon("/usr/share/ukui-panel/panel/img/up.svg"));
    setVisible(false);
}

TrayButton::~TrayButton() { }

void TrayButton::enterEvent(QEvent *) {
    repaint();
    return;
}

void TrayButton::leaveEvent(QEvent *) {
    repaint();
    return;
}
