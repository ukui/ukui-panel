/****************************************************************************
** Meta object code from reading C++ file 'ukuiquicklaunch.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-quicklaunch/ukuiquicklaunch.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuiquicklaunch.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUIQuickLaunch_t {
    QByteArrayData data[22];
    char stringdata0[268];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUIQuickLaunch_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUIQuickLaunch_t qt_meta_stringdata_UKUIQuickLaunch = {
    {
QT_MOC_LITERAL(0, 0, 15), // "UKUIQuickLaunch"
QT_MOC_LITERAL(1, 16, 9), // "addButton"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 18), // "QuickLaunchAction*"
QT_MOC_LITERAL(4, 46, 6), // "action"
QT_MOC_LITERAL(5, 53, 11), // "checkButton"
QT_MOC_LITERAL(6, 65, 8), // "QString*"
QT_MOC_LITERAL(7, 74, 8), // "filename"
QT_MOC_LITERAL(8, 83, 12), // "removeButton"
QT_MOC_LITERAL(9, 96, 13), // "switchButtons"
QT_MOC_LITERAL(10, 110, 18), // "QuickLaunchButton*"
QT_MOC_LITERAL(11, 129, 7), // "button1"
QT_MOC_LITERAL(12, 137, 7), // "button2"
QT_MOC_LITERAL(13, 145, 13), // "buttonDeleted"
QT_MOC_LITERAL(14, 159, 14), // "buttonMoveLeft"
QT_MOC_LITERAL(15, 174, 15), // "buttonMoveRight"
QT_MOC_LITERAL(16, 190, 12), // "AddToTaskbar"
QT_MOC_LITERAL(17, 203, 3), // "arg"
QT_MOC_LITERAL(18, 207, 17), // "RemoveFromTaskbar"
QT_MOC_LITERAL(19, 225, 12), // "CheckIfExist"
QT_MOC_LITERAL(20, 238, 16), // "GetPanelPosition"
QT_MOC_LITERAL(21, 255, 12) // "GetPanelSize"

    },
    "UKUIQuickLaunch\0addButton\0\0"
    "QuickLaunchAction*\0action\0checkButton\0"
    "QString*\0filename\0removeButton\0"
    "switchButtons\0QuickLaunchButton*\0"
    "button1\0button2\0buttonDeleted\0"
    "buttonMoveLeft\0buttonMoveRight\0"
    "AddToTaskbar\0arg\0RemoveFromTaskbar\0"
    "CheckIfExist\0GetPanelPosition\0"
    "GetPanelSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUIQuickLaunch[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x08 /* Private */,
       5,    1,   87,    2, 0x08 /* Private */,
       5,    1,   90,    2, 0x08 /* Private */,
       8,    1,   93,    2, 0x08 /* Private */,
       8,    1,   96,    2, 0x08 /* Private */,
       9,    2,   99,    2, 0x08 /* Private */,
      13,    0,  104,    2, 0x08 /* Private */,
      14,    0,  105,    2, 0x08 /* Private */,
      15,    0,  106,    2, 0x08 /* Private */,
      16,    1,  107,    2, 0x0a /* Public */,
      18,    1,  110,    2, 0x0a /* Public */,
      19,    1,  113,    2, 0x0a /* Public */,
      20,    1,  116,    2, 0x0a /* Public */,
      21,    1,  119,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Bool, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 10,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool, QMetaType::QString,   17,
    QMetaType::Bool, QMetaType::QString,   17,
    QMetaType::Bool, QMetaType::QString,   17,
    QMetaType::Int, QMetaType::QString,   17,
    QMetaType::Int, QMetaType::QString,   17,

       0        // eod
};

void UKUIQuickLaunch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UKUIQuickLaunch *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addButton((*reinterpret_cast< QuickLaunchAction*(*)>(_a[1]))); break;
        case 1: { bool _r = _t->checkButton((*reinterpret_cast< QuickLaunchAction*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->checkButton((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 3: _t->removeButton((*reinterpret_cast< QString*(*)>(_a[1]))); break;
        case 4: _t->removeButton((*reinterpret_cast< QuickLaunchAction*(*)>(_a[1]))); break;
        case 5: _t->switchButtons((*reinterpret_cast< QuickLaunchButton*(*)>(_a[1])),(*reinterpret_cast< QuickLaunchButton*(*)>(_a[2]))); break;
        case 6: _t->buttonDeleted(); break;
        case 7: _t->buttonMoveLeft(); break;
        case 8: _t->buttonMoveRight(); break;
        case 9: { bool _r = _t->AddToTaskbar((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->RemoveFromTaskbar((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->CheckIfExist((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { int _r = _t->GetPanelPosition((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 13: { int _r = _t->GetPanelSize((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUIQuickLaunch::staticMetaObject = { {
    &QFrame::staticMetaObject,
    qt_meta_stringdata_UKUIQuickLaunch.data,
    qt_meta_data_UKUIQuickLaunch,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUIQuickLaunch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUIQuickLaunch::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUIQuickLaunch.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int UKUIQuickLaunch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
struct qt_meta_stringdata_FilectrlAdaptor_t {
    QByteArrayData data[13];
    char stringdata0[979];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FilectrlAdaptor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FilectrlAdaptor_t qt_meta_stringdata_FilectrlAdaptor = {
    {
QT_MOC_LITERAL(0, 0, 15), // "FilectrlAdaptor"
QT_MOC_LITERAL(1, 16, 15), // "D-Bus Interface"
QT_MOC_LITERAL(2, 32, 22), // "com.ukui.panel.desktop"
QT_MOC_LITERAL(3, 55, 19), // "D-Bus Introspection"
QT_MOC_LITERAL(4, 75, 817), // "  <interface name=\"com.ukui...."
QT_MOC_LITERAL(5, 809, 6), // "addtak"
QT_MOC_LITERAL(6, 816, 0), // ""
QT_MOC_LITERAL(7, 817, 12), // "AddToTaskbar"
QT_MOC_LITERAL(8, 830, 3), // "arg"
QT_MOC_LITERAL(9, 834, 12), // "CheckIfExist"
QT_MOC_LITERAL(10, 847, 17), // "RemoveFromTaskbar"
QT_MOC_LITERAL(11, 865, 16), // "GetPanelPosition"
QT_MOC_LITERAL(12, 882, 12) // "GetPanelSize"

    },
    "FilectrlAdaptor\0D-Bus Interface\0"
    "com.ukui.panel.desktop\0D-Bus Introspection\0"
    "  <interface name=\"com.ukui.panel.desktop\">\n    <method name=\"AddT"
    "oTaskbar\">\n      <arg direction=\"out\" type=\"b\"/>\n      <arg dir"
    "ection=\"in\" type=\"s\" name=\"arg\"/>\n    </method>\n    <method na"
    "me=\"RemoveFromTaskbar\">\n      <arg direction=\"out\" type=\"b\"/>\n"
    "      <arg direction=\"in\" type=\"s\" name=\"arg\"/>\n    </method>\n"
    "    <method name=\"CheckIfExist\">\n      <arg direction=\"out\" type="
    "\"b\"/>\n      <arg direction=\"in\" type=\"s\" name=\"arg\"/>\n    </"
    "method>\n    <method name=\"GetPanelPosition\">\n      <arg direction="
    "\"out\" type=\"i\"/>\n      <arg direction=\"in\" type=\"s\" name=\"ar"
    "g\"/>\n    </method>\n    <method name=\"GetPanelSize\">\n      <arg d"
    "irection=\"out\" type=\"i\"/>\n      <arg direction=\"in\" type=\"s\" "
    "name=\"arg\"/>\n    </method>\n  </interface>\n\0"
    "addtak\0\0AddToTaskbar\0arg\0CheckIfExist\0"
    "RemoveFromTaskbar\0GetPanelPosition\0"
    "GetPanelSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FilectrlAdaptor[] = {

 // content:
       8,       // revision
       0,       // classname
       2,   14, // classinfo
       6,   18, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // classinfo: key, value
       1,    2,
       3,    4,

 // signals: name, argc, parameters, tag, flags
       5,    1,   48,    6, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   51,    6, 0x0a /* Public */,
       9,    1,   54,    6, 0x0a /* Public */,
      10,    1,   57,    6, 0x0a /* Public */,
      11,    1,   60,    6, 0x0a /* Public */,
      12,    1,   63,    6, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString,    8,
    QMetaType::Bool, QMetaType::QString,    8,
    QMetaType::Bool, QMetaType::QString,    8,
    QMetaType::Int, QMetaType::QString,    8,
    QMetaType::Int, QMetaType::QString,    8,

       0        // eod
};

void FilectrlAdaptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FilectrlAdaptor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addtak((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: { bool _r = _t->AddToTaskbar((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->CheckIfExist((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { bool _r = _t->RemoveFromTaskbar((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: { int _r = _t->GetPanelPosition((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 5: { int _r = _t->GetPanelSize((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FilectrlAdaptor::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FilectrlAdaptor::addtak)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FilectrlAdaptor::staticMetaObject = { {
    &QDBusAbstractAdaptor::staticMetaObject,
    qt_meta_stringdata_FilectrlAdaptor.data,
    qt_meta_data_FilectrlAdaptor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FilectrlAdaptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FilectrlAdaptor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FilectrlAdaptor.stringdata0))
        return static_cast<void*>(this);
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int FilectrlAdaptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void FilectrlAdaptor::addtak(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
