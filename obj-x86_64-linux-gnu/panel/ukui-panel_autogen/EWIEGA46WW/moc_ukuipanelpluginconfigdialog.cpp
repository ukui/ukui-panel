/****************************************************************************
** Meta object code from reading C++ file 'ukuipanelpluginconfigdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/ukuipanelpluginconfigdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuipanelpluginconfigdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUIPanelPluginConfigDialog_t {
    QByteArrayData data[6];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUIPanelPluginConfigDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUIPanelPluginConfigDialog_t qt_meta_stringdata_UKUIPanelPluginConfigDialog = {
    {
QT_MOC_LITERAL(0, 0, 27), // "UKUIPanelPluginConfigDialog"
QT_MOC_LITERAL(1, 28, 12), // "loadSettings"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 19), // "dialogButtonsAction"
QT_MOC_LITERAL(4, 62, 16), // "QAbstractButton*"
QT_MOC_LITERAL(5, 79, 3) // "btn"

    },
    "UKUIPanelPluginConfigDialog\0loadSettings\0"
    "\0dialogButtonsAction\0QAbstractButton*\0"
    "btn"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUIPanelPluginConfigDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x09 /* Protected */,
       3,    1,   25,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

       0        // eod
};

void UKUIPanelPluginConfigDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UKUIPanelPluginConfigDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadSettings(); break;
        case 1: _t->dialogButtonsAction((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractButton* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UKUIPanelPluginConfigDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_UKUIPanelPluginConfigDialog.data,
    qt_meta_data_UKUIPanelPluginConfigDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUIPanelPluginConfigDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUIPanelPluginConfigDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUIPanelPluginConfigDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int UKUIPanelPluginConfigDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
