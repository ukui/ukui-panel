/****************************************************************************
** Meta object code from reading C++ file 'quicklaunchbutton.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-quicklaunch/quicklaunchbutton.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'quicklaunchbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QuickLaunchButton_t {
    QByteArrayData data[12];
    char stringdata0[142];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QuickLaunchButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QuickLaunchButton_t qt_meta_stringdata_QuickLaunchButton = {
    {
QT_MOC_LITERAL(0, 0, 17), // "QuickLaunchButton"
QT_MOC_LITERAL(1, 18, 13), // "buttonDeleted"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 13), // "switchButtons"
QT_MOC_LITERAL(4, 47, 18), // "QuickLaunchButton*"
QT_MOC_LITERAL(5, 66, 4), // "from"
QT_MOC_LITERAL(6, 71, 2), // "to"
QT_MOC_LITERAL(7, 74, 9), // "movedLeft"
QT_MOC_LITERAL(8, 84, 10), // "movedRight"
QT_MOC_LITERAL(9, 95, 31), // "this_customContextMenuRequested"
QT_MOC_LITERAL(10, 127, 3), // "pos"
QT_MOC_LITERAL(11, 131, 10) // "selfRemove"

    },
    "QuickLaunchButton\0buttonDeleted\0\0"
    "switchButtons\0QuickLaunchButton*\0from\0"
    "to\0movedLeft\0movedRight\0"
    "this_customContextMenuRequested\0pos\0"
    "selfRemove"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QuickLaunchButton[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    2,   45,    2, 0x06 /* Public */,
       7,    0,   50,    2, 0x06 /* Public */,
       8,    0,   51,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   52,    2, 0x08 /* Private */,
      11,    0,   55,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    5,    6,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,   10,
    QMetaType::Void,

       0        // eod
};

void QuickLaunchButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QuickLaunchButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->buttonDeleted(); break;
        case 1: _t->switchButtons((*reinterpret_cast< QuickLaunchButton*(*)>(_a[1])),(*reinterpret_cast< QuickLaunchButton*(*)>(_a[2]))); break;
        case 2: _t->movedLeft(); break;
        case 3: _t->movedRight(); break;
        case 4: _t->this_customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 5: _t->selfRemove(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QuickLaunchButton* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QuickLaunchButton::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuickLaunchButton::buttonDeleted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QuickLaunchButton::*)(QuickLaunchButton * , QuickLaunchButton * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuickLaunchButton::switchButtons)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QuickLaunchButton::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuickLaunchButton::movedLeft)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QuickLaunchButton::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QuickLaunchButton::movedRight)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QuickLaunchButton::staticMetaObject = { {
    &QToolButton::staticMetaObject,
    qt_meta_stringdata_QuickLaunchButton.data,
    qt_meta_data_QuickLaunchButton,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QuickLaunchButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QuickLaunchButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QuickLaunchButton.stringdata0))
        return static_cast<void*>(this);
    return QToolButton::qt_metacast(_clname);
}

int QuickLaunchButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QuickLaunchButton::buttonDeleted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void QuickLaunchButton::switchButtons(QuickLaunchButton * _t1, QuickLaunchButton * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QuickLaunchButton::movedLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QuickLaunchButton::movedRight()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
struct qt_meta_stringdata_ButtonMimeData_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ButtonMimeData_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ButtonMimeData_t qt_meta_stringdata_ButtonMimeData = {
    {
QT_MOC_LITERAL(0, 0, 14) // "ButtonMimeData"

    },
    "ButtonMimeData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ButtonMimeData[] = {

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

void ButtonMimeData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ButtonMimeData::staticMetaObject = { {
    &QMimeData::staticMetaObject,
    qt_meta_stringdata_ButtonMimeData.data,
    qt_meta_data_ButtonMimeData,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ButtonMimeData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ButtonMimeData::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ButtonMimeData.stringdata0))
        return static_cast<void*>(this);
    return QMimeData::qt_metacast(_clname);
}

int ButtonMimeData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMimeData::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
