/****************************************************************************
** Meta object code from reading C++ file 'ukuitaskbutton.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-taskbar/ukuitaskbutton.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuitaskbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUITaskButton_t {
    QByteArrayData data[22];
    char stringdata0[320];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUITaskButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUITaskButton_t qt_meta_stringdata_UKUITaskButton = {
    {
QT_MOC_LITERAL(0, 0, 14), // "UKUITaskButton"
QT_MOC_LITERAL(1, 15, 7), // "dropped"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "dragSource"
QT_MOC_LITERAL(4, 35, 3), // "pos"
QT_MOC_LITERAL(5, 39, 8), // "dragging"
QT_MOC_LITERAL(6, 48, 16), // "raiseApplication"
QT_MOC_LITERAL(7, 65, 19), // "minimizeApplication"
QT_MOC_LITERAL(8, 85, 19), // "maximizeApplication"
QT_MOC_LITERAL(9, 105, 21), // "deMaximizeApplication"
QT_MOC_LITERAL(10, 127, 16), // "shadeApplication"
QT_MOC_LITERAL(11, 144, 18), // "unShadeApplication"
QT_MOC_LITERAL(12, 163, 16), // "closeApplication"
QT_MOC_LITERAL(13, 180, 24), // "moveApplicationToDesktop"
QT_MOC_LITERAL(14, 205, 15), // "moveApplication"
QT_MOC_LITERAL(15, 221, 17), // "resizeApplication"
QT_MOC_LITERAL(16, 239, 19), // "setApplicationLayer"
QT_MOC_LITERAL(17, 259, 9), // "setOrigin"
QT_MOC_LITERAL(18, 269, 10), // "Qt::Corner"
QT_MOC_LITERAL(19, 280, 10), // "updateIcon"
QT_MOC_LITERAL(20, 291, 21), // "activateWithDraggable"
QT_MOC_LITERAL(21, 313, 6) // "origin"

    },
    "UKUITaskButton\0dropped\0\0dragSource\0"
    "pos\0dragging\0raiseApplication\0"
    "minimizeApplication\0maximizeApplication\0"
    "deMaximizeApplication\0shadeApplication\0"
    "unShadeApplication\0closeApplication\0"
    "moveApplicationToDesktop\0moveApplication\0"
    "resizeApplication\0setApplicationLayer\0"
    "setOrigin\0Qt::Corner\0updateIcon\0"
    "activateWithDraggable\0origin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUITaskButton[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       1,  120, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   94,    2, 0x06 /* Public */,
       5,    2,   99,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,  104,    2, 0x0a /* Public */,
       7,    0,  105,    2, 0x0a /* Public */,
       8,    0,  106,    2, 0x0a /* Public */,
       9,    0,  107,    2, 0x0a /* Public */,
      10,    0,  108,    2, 0x0a /* Public */,
      11,    0,  109,    2, 0x0a /* Public */,
      12,    0,  110,    2, 0x0a /* Public */,
      13,    0,  111,    2, 0x0a /* Public */,
      14,    0,  112,    2, 0x0a /* Public */,
      15,    0,  113,    2, 0x0a /* Public */,
      16,    0,  114,    2, 0x0a /* Public */,
      17,    1,  115,    2, 0x0a /* Public */,
      19,    0,  118,    2, 0x0a /* Public */,
      20,    0,  119,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QObjectStar, QMetaType::QPoint,    3,    4,
    QMetaType::Void, QMetaType::QObjectStar, QMetaType::QPoint,    3,    4,

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
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      21, 0x80000000 | 18, 0x0009510b,

       0        // eod
};

void UKUITaskButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UKUITaskButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dropped((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 1: _t->dragging((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 2: _t->raiseApplication(); break;
        case 3: _t->minimizeApplication(); break;
        case 4: _t->maximizeApplication(); break;
        case 5: _t->deMaximizeApplication(); break;
        case 6: _t->shadeApplication(); break;
        case 7: _t->unShadeApplication(); break;
        case 8: _t->closeApplication(); break;
        case 9: _t->moveApplicationToDesktop(); break;
        case 10: _t->moveApplication(); break;
        case 11: _t->resizeApplication(); break;
        case 12: _t->setApplicationLayer(); break;
        case 13: _t->setOrigin((*reinterpret_cast< Qt::Corner(*)>(_a[1]))); break;
        case 14: _t->updateIcon(); break;
        case 15: _t->activateWithDraggable(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UKUITaskButton::*)(QObject * , QPoint const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UKUITaskButton::dropped)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UKUITaskButton::*)(QObject * , QPoint const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UKUITaskButton::dragging)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<UKUITaskButton *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Corner*>(_v) = _t->origin(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<UKUITaskButton *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setOrigin(*reinterpret_cast< Qt::Corner*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject UKUITaskButton::staticMetaObject = { {
    &QToolButton::staticMetaObject,
    qt_meta_stringdata_UKUITaskButton.data,
    qt_meta_data_UKUITaskButton,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUITaskButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUITaskButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUITaskButton.stringdata0))
        return static_cast<void*>(this);
    return QToolButton::qt_metacast(_clname);
}

int UKUITaskButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
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
void UKUITaskButton::dropped(QObject * _t1, QPoint const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UKUITaskButton::dragging(QObject * _t1, QPoint const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
