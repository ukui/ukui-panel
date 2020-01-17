/****************************************************************************
** Meta object code from reading C++ file 'showdesktop.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-showdesktop/showdesktop.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'showdesktop.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ShowDesktop_t {
    QByteArrayData data[3];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ShowDesktop_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ShowDesktop_t qt_meta_stringdata_ShowDesktop = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ShowDesktop"
QT_MOC_LITERAL(1, 12, 20), // "toggleShowingDesktop"
QT_MOC_LITERAL(2, 33, 0) // ""

    },
    "ShowDesktop\0toggleShowingDesktop\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ShowDesktop[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void ShowDesktop::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ShowDesktop *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->toggleShowingDesktop(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ShowDesktop::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_ShowDesktop.data,
    qt_meta_data_ShowDesktop,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ShowDesktop::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShowDesktop::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ShowDesktop.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IUKUIPanelPlugin"))
        return static_cast< IUKUIPanelPlugin*>(this);
    return QObject::qt_metacast(_clname);
}

int ShowDesktop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_ShowDesktopLibrary_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ShowDesktopLibrary_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ShowDesktopLibrary_t qt_meta_stringdata_ShowDesktopLibrary = {
    {
QT_MOC_LITERAL(0, 0, 18) // "ShowDesktopLibrary"

    },
    "ShowDesktopLibrary"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ShowDesktopLibrary[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ShowDesktopLibrary::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ShowDesktopLibrary::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_ShowDesktopLibrary.data,
    qt_meta_data_ShowDesktopLibrary,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ShowDesktopLibrary::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShowDesktopLibrary::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ShowDesktopLibrary.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IUKUIPanelPluginLibrary"))
        return static_cast< IUKUIPanelPluginLibrary*>(this);
    if (!strcmp(_clname, "ukui.org/Panel/PluginInterface/3.0"))
        return static_cast< IUKUIPanelPluginLibrary*>(this);
    return QObject::qt_metacast(_clname);
}

int ShowDesktopLibrary::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}

QT_PLUGIN_METADATA_SECTION
static constexpr unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x22,  'u',  'k',  'u',  'i',  '.', 
    'o',  'r',  'g',  '/',  'P',  'a',  'n',  'e', 
    'l',  '/',  'P',  'l',  'u',  'g',  'i',  'n', 
    'I',  'n',  't',  'e',  'r',  'f',  'a',  'c', 
    'e',  '/',  '3',  '.',  '0', 
    // "className"
    0x03,  0x72,  'S',  'h',  'o',  'w',  'D',  'e', 
    's',  'k',  't',  'o',  'p',  'L',  'i',  'b', 
    'r',  'a',  'r',  'y', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(ShowDesktopLibrary, ShowDesktopLibrary)

QT_WARNING_POP
QT_END_MOC_NAMESPACE
