/****************************************************************************
** Meta object code from reading C++ file 'spacer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-spacer/spacer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spacer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SpacerWidget_t {
    QByteArrayData data[3];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpacerWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpacerWidget_t qt_meta_stringdata_SpacerWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SpacerWidget"
QT_MOC_LITERAL(1, 13, 4), // "type"
QT_MOC_LITERAL(2, 18, 11) // "orientation"

    },
    "SpacerWidget\0type\0orientation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpacerWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095001,
       2, QMetaType::QString, 0x00095001,

       0        // eod
};

void SpacerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<SpacerWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getType(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->getOrientation(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject SpacerWidget::staticMetaObject = { {
    &QFrame::staticMetaObject,
    qt_meta_stringdata_SpacerWidget.data,
    qt_meta_data_SpacerWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpacerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpacerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpacerWidget.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int SpacerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_Spacer_t {
    QByteArrayData data[3];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Spacer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Spacer_t qt_meta_stringdata_Spacer = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Spacer"
QT_MOC_LITERAL(1, 7, 15), // "settingsChanged"
QT_MOC_LITERAL(2, 23, 0) // ""

    },
    "Spacer\0settingsChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Spacer[] = {

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

void Spacer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Spacer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->settingsChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Spacer::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Spacer.data,
    qt_meta_data_Spacer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Spacer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Spacer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Spacer.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IUKUIPanelPlugin"))
        return static_cast< IUKUIPanelPlugin*>(this);
    return QObject::qt_metacast(_clname);
}

int Spacer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_SpacerPluginLibrary_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpacerPluginLibrary_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpacerPluginLibrary_t qt_meta_stringdata_SpacerPluginLibrary = {
    {
QT_MOC_LITERAL(0, 0, 19) // "SpacerPluginLibrary"

    },
    "SpacerPluginLibrary"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpacerPluginLibrary[] = {

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

void SpacerPluginLibrary::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject SpacerPluginLibrary::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_SpacerPluginLibrary.data,
    qt_meta_data_SpacerPluginLibrary,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpacerPluginLibrary::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpacerPluginLibrary::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpacerPluginLibrary.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IUKUIPanelPluginLibrary"))
        return static_cast< IUKUIPanelPluginLibrary*>(this);
    if (!strcmp(_clname, "ukui.org/Panel/PluginInterface/3.0"))
        return static_cast< IUKUIPanelPluginLibrary*>(this);
    return QObject::qt_metacast(_clname);
}

int SpacerPluginLibrary::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
