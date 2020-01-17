/****************************************************************************
** Meta object code from reading C++ file 'plugin.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/plugin.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Plugin_t {
    QByteArrayData data[10];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Plugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Plugin_t qt_meta_stringdata_Plugin = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Plugin"
QT_MOC_LITERAL(1, 7, 9), // "startMove"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 6), // "remove"
QT_MOC_LITERAL(4, 25, 8), // "dragLeft"
QT_MOC_LITERAL(5, 34, 7), // "realign"
QT_MOC_LITERAL(6, 42, 19), // "showConfigureDialog"
QT_MOC_LITERAL(7, 62, 13), // "requestRemove"
QT_MOC_LITERAL(8, 76, 15), // "settingsChanged"
QT_MOC_LITERAL(9, 92, 15) // "moveMarkerColor"

    },
    "Plugin\0startMove\0\0remove\0dragLeft\0"
    "realign\0showConfigureDialog\0requestRemove\0"
    "settingsChanged\0moveMarkerColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Plugin[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       1,   56, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    0,   51,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   52,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x0a /* Public */,
       7,    0,   54,    2, 0x0a /* Public */,
       8,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::QColor, 0x00095103,

       0        // eod
};

void Plugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Plugin *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startMove(); break;
        case 1: _t->remove(); break;
        case 2: _t->dragLeft(); break;
        case 3: _t->realign(); break;
        case 4: _t->showConfigureDialog(); break;
        case 5: _t->requestRemove(); break;
        case 6: _t->settingsChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Plugin::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Plugin::startMove)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Plugin::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Plugin::remove)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Plugin::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Plugin::dragLeft)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Plugin *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->moveMarkerColor(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Plugin *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setMoveMarkerColor(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Plugin::staticMetaObject = { {
    &QFrame::staticMetaObject,
    qt_meta_stringdata_Plugin.data,
    qt_meta_data_Plugin,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Plugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Plugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Plugin.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int Plugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Plugin::startMove()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Plugin::remove()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Plugin::dragLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
