/****************************************************************************
** Meta object code from reading C++ file 'ukuiapplication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/common/ukuiapplication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuiapplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUi__Application_t {
    QByteArrayData data[6];
    char stringdata0[61];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUi__Application_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUi__Application_t qt_meta_stringdata_UKUi__Application = {
    {
QT_MOC_LITERAL(0, 0, 17), // "UKUi::Application"
QT_MOC_LITERAL(1, 18, 12), // "themeChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 10), // "unixSignal"
QT_MOC_LITERAL(4, 43, 5), // "signo"
QT_MOC_LITERAL(5, 49, 11) // "updateTheme"

    },
    "UKUi::Application\0themeChanged\0\0"
    "unixSignal\0signo\0updateTheme"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUi__Application[] = {

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
       3,    1,   30,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   33,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void UKUi::Application::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Application *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->themeChanged(); break;
        case 1: _t->unixSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->updateTheme(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Application::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Application::themeChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Application::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Application::unixSignal)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUi::Application::staticMetaObject = { {
    &QApplication::staticMetaObject,
    qt_meta_stringdata_UKUi__Application.data,
    qt_meta_data_UKUi__Application,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUi::Application::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUi::Application::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUi__Application.stringdata0))
        return static_cast<void*>(this);
    return QApplication::qt_metacast(_clname);
}

int UKUi::Application::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
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
void UKUi::Application::themeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UKUi::Application::unixSignal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
