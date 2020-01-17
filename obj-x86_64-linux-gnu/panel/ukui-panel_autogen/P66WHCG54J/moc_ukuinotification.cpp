/****************************************************************************
** Meta object code from reading C++ file 'ukuinotification.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/common/ukuinotification.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuinotification.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUi__Notification_t {
    QByteArrayData data[9];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUi__Notification_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUi__Notification_t qt_meta_stringdata_UKUi__Notification = {
    {
QT_MOC_LITERAL(0, 0, 18), // "UKUi::Notification"
QT_MOC_LITERAL(1, 19, 18), // "notificationClosed"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 31), // "UKUi::Notification::CloseReason"
QT_MOC_LITERAL(4, 71, 6), // "reason"
QT_MOC_LITERAL(5, 78, 15), // "actionActivated"
QT_MOC_LITERAL(6, 94, 12), // "actionNumber"
QT_MOC_LITERAL(7, 107, 6), // "update"
QT_MOC_LITERAL(8, 114, 5) // "close"

    },
    "UKUi::Notification\0notificationClosed\0"
    "\0UKUi::Notification::CloseReason\0"
    "reason\0actionActivated\0actionNumber\0"
    "update\0close"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUi__Notification[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   40,    2, 0x0a /* Public */,
       8,    0,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UKUi::Notification::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Notification *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->notificationClosed((*reinterpret_cast< UKUi::Notification::CloseReason(*)>(_a[1]))); break;
        case 1: _t->actionActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->update(); break;
        case 3: _t->close(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Notification::*)(UKUi::Notification::CloseReason );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Notification::notificationClosed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Notification::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Notification::actionActivated)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUi::Notification::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_UKUi__Notification.data,
    qt_meta_data_UKUi__Notification,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUi::Notification::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUi::Notification::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUi__Notification.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UKUi::Notification::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void UKUi::Notification::notificationClosed(UKUi::Notification::CloseReason _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UKUi::Notification::actionActivated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
