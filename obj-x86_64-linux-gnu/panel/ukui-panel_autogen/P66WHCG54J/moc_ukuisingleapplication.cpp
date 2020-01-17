/****************************************************************************
** Meta object code from reading C++ file 'ukuisingleapplication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/common/ukuisingleapplication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuisingleapplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUi__SingleApplication_t {
    QByteArrayData data[6];
    char stringdata0[91];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUi__SingleApplication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUi__SingleApplication_t qt_meta_stringdata_UKUi__SingleApplication = {
    {
QT_MOC_LITERAL(0, 0, 23), // "UKUi::SingleApplication"
QT_MOC_LITERAL(1, 24, 14), // "activateWindow"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 12), // "StartOptions"
QT_MOC_LITERAL(4, 53, 17), // "ExitOnDBusFailure"
QT_MOC_LITERAL(5, 71, 19) // "NoExitOnDBusFailure"

    },
    "UKUi::SingleApplication\0activateWindow\0"
    "\0StartOptions\0ExitOnDBusFailure\0"
    "NoExitOnDBusFailure"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUi__SingleApplication[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       1,   20, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,

 // enums: name, alias, flags, count, data
       3,    3, 0x0,    2,   25,

 // enum data: key, value
       4, uint(UKUi::SingleApplication::ExitOnDBusFailure),
       5, uint(UKUi::SingleApplication::NoExitOnDBusFailure),

       0        // eod
};

void UKUi::SingleApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SingleApplication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->activateWindow(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject UKUi::SingleApplication::staticMetaObject = { {
    &Application::staticMetaObject,
    qt_meta_stringdata_UKUi__SingleApplication.data,
    qt_meta_data_UKUi__SingleApplication,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUi::SingleApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUi::SingleApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUi__SingleApplication.stringdata0))
        return static_cast<void*>(this);
    return Application::qt_metacast(_clname);
}

int UKUi::SingleApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Application::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
