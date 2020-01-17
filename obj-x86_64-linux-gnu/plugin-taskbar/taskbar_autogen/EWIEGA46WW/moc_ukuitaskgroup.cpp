/****************************************************************************
** Meta object code from reading C++ file 'ukuitaskgroup.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-taskbar/ukuitaskgroup.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuitaskgroup.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUITaskGroup_t {
    QByteArrayData data[22];
    char stringdata0[274];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUITaskGroup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUITaskGroup_t qt_meta_stringdata_UKUITaskGroup = {
    {
QT_MOC_LITERAL(0, 0, 13), // "UKUITaskGroup"
QT_MOC_LITERAL(1, 14, 16), // "groupBecomeEmpty"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "name"
QT_MOC_LITERAL(4, 37, 17), // "visibilityChanged"
QT_MOC_LITERAL(5, 55, 7), // "visible"
QT_MOC_LITERAL(6, 63, 10), // "popupShown"
QT_MOC_LITERAL(7, 74, 14), // "UKUITaskGroup*"
QT_MOC_LITERAL(8, 89, 6), // "sender"
QT_MOC_LITERAL(9, 96, 15), // "onWindowRemoved"
QT_MOC_LITERAL(10, 112, 3), // "WId"
QT_MOC_LITERAL(11, 116, 6), // "window"
QT_MOC_LITERAL(12, 123, 9), // "onClicked"
QT_MOC_LITERAL(13, 133, 7), // "checked"
QT_MOC_LITERAL(14, 141, 20), // "onChildButtonClicked"
QT_MOC_LITERAL(15, 162, 21), // "onActiveWindowChanged"
QT_MOC_LITERAL(16, 184, 16), // "onDesktopChanged"
QT_MOC_LITERAL(17, 201, 6), // "number"
QT_MOC_LITERAL(18, 208, 10), // "closeGroup"
QT_MOC_LITERAL(19, 219, 20), // "refreshIconsGeometry"
QT_MOC_LITERAL(20, 240, 17), // "refreshVisibility"
QT_MOC_LITERAL(21, 258, 15) // "groupPopupShown"

    },
    "UKUITaskGroup\0groupBecomeEmpty\0\0name\0"
    "visibilityChanged\0visible\0popupShown\0"
    "UKUITaskGroup*\0sender\0onWindowRemoved\0"
    "WId\0window\0onClicked\0checked\0"
    "onChildButtonClicked\0onActiveWindowChanged\0"
    "onDesktopChanged\0number\0closeGroup\0"
    "refreshIconsGeometry\0refreshVisibility\0"
    "groupPopupShown"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUITaskGroup[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       4,    1,   77,    2, 0x06 /* Public */,
       6,    1,   80,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   83,    2, 0x0a /* Public */,
      12,    1,   86,    2, 0x08 /* Private */,
      14,    0,   89,    2, 0x08 /* Private */,
      15,    1,   90,    2, 0x08 /* Private */,
      16,    1,   93,    2, 0x08 /* Private */,
      18,    0,   96,    2, 0x08 /* Private */,
      19,    0,   97,    2, 0x08 /* Private */,
      20,    0,   98,    2, 0x08 /* Private */,
      21,    1,   99,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, 0x80000000 | 7,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void UKUITaskGroup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UKUITaskGroup *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->groupBecomeEmpty((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->visibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->popupShown((*reinterpret_cast< UKUITaskGroup*(*)>(_a[1]))); break;
        case 3: _t->onWindowRemoved((*reinterpret_cast< WId(*)>(_a[1]))); break;
        case 4: _t->onClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->onChildButtonClicked(); break;
        case 6: _t->onActiveWindowChanged((*reinterpret_cast< WId(*)>(_a[1]))); break;
        case 7: _t->onDesktopChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->closeGroup(); break;
        case 9: _t->refreshIconsGeometry(); break;
        case 10: _t->refreshVisibility(); break;
        case 11: _t->groupPopupShown((*reinterpret_cast< UKUITaskGroup*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< UKUITaskGroup* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< UKUITaskGroup* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UKUITaskGroup::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UKUITaskGroup::groupBecomeEmpty)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UKUITaskGroup::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UKUITaskGroup::visibilityChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (UKUITaskGroup::*)(UKUITaskGroup * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UKUITaskGroup::popupShown)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUITaskGroup::staticMetaObject = { {
    &UKUITaskButton::staticMetaObject,
    qt_meta_stringdata_UKUITaskGroup.data,
    qt_meta_data_UKUITaskGroup,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUITaskGroup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUITaskGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUITaskGroup.stringdata0))
        return static_cast<void*>(this);
    return UKUITaskButton::qt_metacast(_clname);
}

int UKUITaskGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = UKUITaskButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void UKUITaskGroup::groupBecomeEmpty(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UKUITaskGroup::visibilityChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void UKUITaskGroup::popupShown(UKUITaskGroup * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
