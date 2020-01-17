/****************************************************************************
** Meta object code from reading C++ file 'ukuisettings.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/common/ukuisettings.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuisettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUi__Settings_t {
    QByteArrayData data[8];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUi__Settings_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUi__Settings_t qt_meta_stringdata_UKUi__Settings = {
    {
QT_MOC_LITERAL(0, 0, 14), // "UKUi::Settings"
QT_MOC_LITERAL(1, 15, 15), // "settingsChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 27), // "settingsChangedFromExternal"
QT_MOC_LITERAL(4, 60, 20), // "settingsChangedByApp"
QT_MOC_LITERAL(5, 81, 11), // "fileChanged"
QT_MOC_LITERAL(6, 93, 12), // "_fileChanged"
QT_MOC_LITERAL(7, 106, 4) // "path"

    },
    "UKUi::Settings\0settingsChanged\0\0"
    "settingsChangedFromExternal\0"
    "settingsChangedByApp\0fileChanged\0"
    "_fileChanged\0path"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUi__Settings[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   42,    2, 0x09 /* Protected */,
       6,    1,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void UKUi::Settings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Settings *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->settingsChanged(); break;
        case 1: _t->settingsChangedFromExternal(); break;
        case 2: _t->settingsChangedByApp(); break;
        case 3: _t->fileChanged(); break;
        case 4: _t->_fileChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Settings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::settingsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Settings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::settingsChangedFromExternal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Settings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::settingsChangedByApp)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUi::Settings::staticMetaObject = { {
    &QSettings::staticMetaObject,
    qt_meta_stringdata_UKUi__Settings.data,
    qt_meta_data_UKUi__Settings,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUi::Settings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUi::Settings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUi__Settings.stringdata0))
        return static_cast<void*>(this);
    return QSettings::qt_metacast(_clname);
}

int UKUi::Settings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSettings::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void UKUi::Settings::settingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UKUi::Settings::settingsChangedFromExternal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void UKUi::Settings::settingsChangedByApp()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_UKUi__GlobalSettings_t {
    QByteArrayData data[5];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUi__GlobalSettings_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUi__GlobalSettings_t qt_meta_stringdata_UKUi__GlobalSettings = {
    {
QT_MOC_LITERAL(0, 0, 20), // "UKUi::GlobalSettings"
QT_MOC_LITERAL(1, 21, 16), // "iconThemeChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 16), // "ukuiThemeChanged"
QT_MOC_LITERAL(4, 56, 11) // "fileChanged"

    },
    "UKUi::GlobalSettings\0iconThemeChanged\0"
    "\0ukuiThemeChanged\0fileChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUi__GlobalSettings[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   31,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void UKUi::GlobalSettings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GlobalSettings *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->iconThemeChanged(); break;
        case 1: _t->ukuiThemeChanged(); break;
        case 2: _t->fileChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GlobalSettings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GlobalSettings::iconThemeChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GlobalSettings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GlobalSettings::ukuiThemeChanged)) {
                *result = 1;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject UKUi::GlobalSettings::staticMetaObject = { {
    &Settings::staticMetaObject,
    qt_meta_stringdata_UKUi__GlobalSettings.data,
    qt_meta_data_UKUi__GlobalSettings,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUi::GlobalSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUi::GlobalSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUi__GlobalSettings.stringdata0))
        return static_cast<void*>(this);
    return Settings::qt_metacast(_clname);
}

int UKUi::GlobalSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Settings::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void UKUi::GlobalSettings::iconThemeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UKUi::GlobalSettings::ukuiThemeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
