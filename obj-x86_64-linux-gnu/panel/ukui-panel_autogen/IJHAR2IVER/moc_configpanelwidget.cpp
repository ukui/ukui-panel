/****************************************************************************
** Meta object code from reading C++ file 'configpanelwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/config/configpanelwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'configpanelwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConfigPanelWidget_t {
    QByteArrayData data[18];
    char stringdata0[287];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConfigPanelWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConfigPanelWidget_t qt_meta_stringdata_ConfigPanelWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ConfigPanelWidget"
QT_MOC_LITERAL(1, 18, 7), // "changed"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 5), // "reset"
QT_MOC_LITERAL(4, 33, 19), // "positionChanged_top"
QT_MOC_LITERAL(5, 53, 22), // "positionChanged_bottom"
QT_MOC_LITERAL(6, 76, 20), // "positionChanged_left"
QT_MOC_LITERAL(7, 97, 21), // "positionChanged_right"
QT_MOC_LITERAL(8, 119, 15), // "changeToMidSize"
QT_MOC_LITERAL(9, 135, 17), // "changeToLargeSize"
QT_MOC_LITERAL(10, 153, 17), // "changeToSmallSize"
QT_MOC_LITERAL(11, 171, 16), // "changeBackground"
QT_MOC_LITERAL(12, 188, 11), // "editChanged"
QT_MOC_LITERAL(13, 200, 16), // "widthTypeChanged"
QT_MOC_LITERAL(14, 217, 15), // "positionChanged"
QT_MOC_LITERAL(15, 233, 13), // "pickFontColor"
QT_MOC_LITERAL(16, 247, 19), // "pickBackgroundColor"
QT_MOC_LITERAL(17, 267, 19) // "pickBackgroundImage"

    },
    "ConfigPanelWidget\0changed\0\0reset\0"
    "positionChanged_top\0positionChanged_bottom\0"
    "positionChanged_left\0positionChanged_right\0"
    "changeToMidSize\0changeToLargeSize\0"
    "changeToSmallSize\0changeBackground\0"
    "editChanged\0widthTypeChanged\0"
    "positionChanged\0pickFontColor\0"
    "pickBackgroundColor\0pickBackgroundImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConfigPanelWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   95,    2, 0x0a /* Public */,
       4,    0,   96,    2, 0x0a /* Public */,
       5,    0,   97,    2, 0x0a /* Public */,
       6,    0,   98,    2, 0x0a /* Public */,
       7,    0,   99,    2, 0x0a /* Public */,
       8,    0,  100,    2, 0x0a /* Public */,
       9,    0,  101,    2, 0x0a /* Public */,
      10,    0,  102,    2, 0x0a /* Public */,
      11,    0,  103,    2, 0x0a /* Public */,
      12,    0,  104,    2, 0x08 /* Private */,
      13,    0,  105,    2, 0x08 /* Private */,
      14,    0,  106,    2, 0x08 /* Private */,
      15,    0,  107,    2, 0x08 /* Private */,
      16,    0,  108,    2, 0x08 /* Private */,
      17,    0,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ConfigPanelWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConfigPanelWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changed(); break;
        case 1: _t->reset(); break;
        case 2: _t->positionChanged_top(); break;
        case 3: _t->positionChanged_bottom(); break;
        case 4: _t->positionChanged_left(); break;
        case 5: _t->positionChanged_right(); break;
        case 6: _t->changeToMidSize(); break;
        case 7: _t->changeToLargeSize(); break;
        case 8: _t->changeToSmallSize(); break;
        case 9: _t->changeBackground(); break;
        case 10: _t->editChanged(); break;
        case 11: _t->widthTypeChanged(); break;
        case 12: _t->positionChanged(); break;
        case 13: _t->pickFontColor(); break;
        case 14: _t->pickBackgroundColor(); break;
        case 15: _t->pickBackgroundImage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConfigPanelWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConfigPanelWidget::changed)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ConfigPanelWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_ConfigPanelWidget.data,
    qt_meta_data_ConfigPanelWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ConfigPanelWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConfigPanelWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ConfigPanelWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ConfigPanelWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void ConfigPanelWidget::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
