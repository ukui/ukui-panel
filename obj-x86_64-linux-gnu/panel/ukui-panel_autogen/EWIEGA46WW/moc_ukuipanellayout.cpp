/****************************************************************************
** Meta object code from reading C++ file 'ukuipanellayout.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/ukuipanellayout.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuipanellayout.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUIPanelLayout_t {
    QByteArrayData data[9];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUIPanelLayout_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUIPanelLayout_t qt_meta_stringdata_UKUIPanelLayout = {
    {
QT_MOC_LITERAL(0, 0, 15), // "UKUIPanelLayout"
QT_MOC_LITERAL(1, 16, 11), // "pluginMoved"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 7), // "Plugin*"
QT_MOC_LITERAL(4, 37, 6), // "plugin"
QT_MOC_LITERAL(5, 44, 15), // "startMovePlugin"
QT_MOC_LITERAL(6, 60, 16), // "finishMovePlugin"
QT_MOC_LITERAL(7, 77, 12), // "moveUpPlugin"
QT_MOC_LITERAL(8, 90, 9) // "addPlugin"

    },
    "UKUIPanelLayout\0pluginMoved\0\0Plugin*\0"
    "plugin\0startMovePlugin\0finishMovePlugin\0"
    "moveUpPlugin\0addPlugin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUIPanelLayout[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   42,    2, 0x0a /* Public */,
       6,    0,   43,    2, 0x0a /* Public */,
       7,    1,   44,    2, 0x0a /* Public */,
       8,    1,   47,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void UKUIPanelLayout::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UKUIPanelLayout *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pluginMoved((*reinterpret_cast< Plugin*(*)>(_a[1]))); break;
        case 1: _t->startMovePlugin(); break;
        case 2: _t->finishMovePlugin(); break;
        case 3: _t->moveUpPlugin((*reinterpret_cast< Plugin*(*)>(_a[1]))); break;
        case 4: _t->addPlugin((*reinterpret_cast< Plugin*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UKUIPanelLayout::*)(Plugin * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UKUIPanelLayout::pluginMoved)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUIPanelLayout::staticMetaObject = { {
    &QLayout::staticMetaObject,
    qt_meta_stringdata_UKUIPanelLayout.data,
    qt_meta_data_UKUIPanelLayout,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUIPanelLayout::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUIPanelLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUIPanelLayout.stringdata0))
        return static_cast<void*>(this);
    return QLayout::qt_metacast(_clname);
}

int UKUIPanelLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLayout::qt_metacall(_c, _id, _a);
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
void UKUIPanelLayout::pluginMoved(Plugin * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
