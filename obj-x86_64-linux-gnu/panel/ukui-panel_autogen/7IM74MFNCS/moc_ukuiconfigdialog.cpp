/****************************************************************************
** Meta object code from reading C++ file 'ukuiconfigdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/common/configdialog/ukuiconfigdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuiconfigdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUi__ConfigDialog_t {
    QByteArrayData data[6];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUi__ConfigDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUi__ConfigDialog_t qt_meta_stringdata_UKUi__ConfigDialog = {
    {
QT_MOC_LITERAL(0, 0, 18), // "UKUi::ConfigDialog"
QT_MOC_LITERAL(1, 19, 5), // "reset"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 4), // "save"
QT_MOC_LITERAL(4, 31, 7), // "clicked"
QT_MOC_LITERAL(5, 39, 32) // "QDialogButtonBox::StandardButton"

    },
    "UKUi::ConfigDialog\0reset\0\0save\0clicked\0"
    "QDialogButtonBox::StandardButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUi__ConfigDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    1,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,

       0        // eod
};

void UKUi::ConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConfigDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->reset(); break;
        case 1: _t->save(); break;
        case 2: _t->clicked((*reinterpret_cast< QDialogButtonBox::StandardButton(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConfigDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConfigDialog::reset)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConfigDialog::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConfigDialog::save)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ConfigDialog::*)(QDialogButtonBox::StandardButton );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConfigDialog::clicked)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUi::ConfigDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_UKUi__ConfigDialog.data,
    qt_meta_data_UKUi__ConfigDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUi::ConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUi::ConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUi__ConfigDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int UKUi::ConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
void UKUi::ConfigDialog::reset()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UKUi::ConfigDialog::save()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void UKUi::ConfigDialog::clicked(QDialogButtonBox::StandardButton _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
