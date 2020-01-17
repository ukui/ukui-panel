/****************************************************************************
** Meta object code from reading C++ file 'spacerconfiguration.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-spacer/spacerconfiguration.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spacerconfiguration.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SpacerConfiguration_t {
    QByteArrayData data[9];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpacerConfiguration_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpacerConfiguration_t qt_meta_stringdata_SpacerConfiguration = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SpacerConfiguration"
QT_MOC_LITERAL(1, 20, 12), // "loadSettings"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 11), // "sizeChanged"
QT_MOC_LITERAL(4, 46, 5), // "value"
QT_MOC_LITERAL(5, 52, 11), // "typeChanged"
QT_MOC_LITERAL(6, 64, 5), // "index"
QT_MOC_LITERAL(7, 70, 16), // "widthTypeChanged"
QT_MOC_LITERAL(8, 87, 17) // "expandableChecked"

    },
    "SpacerConfiguration\0loadSettings\0\0"
    "sizeChanged\0value\0typeChanged\0index\0"
    "widthTypeChanged\0expandableChecked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpacerConfiguration[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    1,   35,    2, 0x08 /* Private */,
       5,    1,   38,    2, 0x08 /* Private */,
       7,    1,   41,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Bool,    8,

       0        // eod
};

void SpacerConfiguration::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SpacerConfiguration *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadSettings(); break;
        case 1: _t->sizeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->typeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->widthTypeChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SpacerConfiguration::staticMetaObject = { {
    &UKUIPanelPluginConfigDialog::staticMetaObject,
    qt_meta_stringdata_SpacerConfiguration.data,
    qt_meta_data_SpacerConfiguration,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpacerConfiguration::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpacerConfiguration::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpacerConfiguration.stringdata0))
        return static_cast<void*>(this);
    return UKUIPanelPluginConfigDialog::qt_metacast(_clname);
}

int SpacerConfiguration::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = UKUIPanelPluginConfigDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
