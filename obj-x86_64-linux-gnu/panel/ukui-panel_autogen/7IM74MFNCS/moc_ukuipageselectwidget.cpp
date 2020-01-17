/****************************************************************************
** Meta object code from reading C++ file 'ukuipageselectwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/common/configdialog/ukuipageselectwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuipageselectwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UKUi__PageSelectWidget_t {
    QByteArrayData data[3];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUi__PageSelectWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUi__PageSelectWidget_t qt_meta_stringdata_UKUi__PageSelectWidget = {
    {
QT_MOC_LITERAL(0, 0, 22), // "UKUi::PageSelectWidget"
QT_MOC_LITERAL(1, 23, 18), // "updateMaxTextWidth"
QT_MOC_LITERAL(2, 42, 0) // ""

    },
    "UKUi::PageSelectWidget\0updateMaxTextWidth\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUi__PageSelectWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void UKUi::PageSelectWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PageSelectWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateMaxTextWidth(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject UKUi::PageSelectWidget::staticMetaObject = { {
    &QListWidget::staticMetaObject,
    qt_meta_stringdata_UKUi__PageSelectWidget.data,
    qt_meta_data_UKUi__PageSelectWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUi::PageSelectWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUi::PageSelectWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUi__PageSelectWidget.stringdata0))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int UKUi::PageSelectWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
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
