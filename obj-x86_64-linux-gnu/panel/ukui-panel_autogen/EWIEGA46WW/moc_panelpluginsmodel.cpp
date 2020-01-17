/****************************************************************************
** Meta object code from reading C++ file 'panelpluginsmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../panel/panelpluginsmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'panelpluginsmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PanelPluginsModel_t {
    QByteArrayData data[18];
    char stringdata0[221];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PanelPluginsModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PanelPluginsModel_t qt_meta_stringdata_PanelPluginsModel = {
    {
QT_MOC_LITERAL(0, 0, 17), // "PanelPluginsModel"
QT_MOC_LITERAL(1, 18, 11), // "pluginAdded"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 7), // "Plugin*"
QT_MOC_LITERAL(4, 39, 6), // "plugin"
QT_MOC_LITERAL(5, 46, 13), // "pluginRemoved"
QT_MOC_LITERAL(6, 60, 11), // "pluginMoved"
QT_MOC_LITERAL(7, 72, 13), // "pluginMovedUp"
QT_MOC_LITERAL(8, 86, 9), // "addPlugin"
QT_MOC_LITERAL(9, 96, 16), // "UKUi::PluginInfo"
QT_MOC_LITERAL(10, 113, 11), // "desktopFile"
QT_MOC_LITERAL(11, 125, 12), // "removePlugin"
QT_MOC_LITERAL(12, 138, 14), // "onMovePluginUp"
QT_MOC_LITERAL(13, 153, 11), // "QModelIndex"
QT_MOC_LITERAL(14, 165, 5), // "index"
QT_MOC_LITERAL(15, 171, 16), // "onMovePluginDown"
QT_MOC_LITERAL(16, 188, 17), // "onConfigurePlugin"
QT_MOC_LITERAL(17, 206, 14) // "onRemovePlugin"

    },
    "PanelPluginsModel\0pluginAdded\0\0Plugin*\0"
    "plugin\0pluginRemoved\0pluginMoved\0"
    "pluginMovedUp\0addPlugin\0UKUi::PluginInfo\0"
    "desktopFile\0removePlugin\0onMovePluginUp\0"
    "QModelIndex\0index\0onMovePluginDown\0"
    "onConfigurePlugin\0onRemovePlugin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PanelPluginsModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       5,    1,   67,    2, 0x06 /* Public */,
       6,    1,   70,    2, 0x06 /* Public */,
       7,    1,   73,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   76,    2, 0x0a /* Public */,
      11,    0,   79,    2, 0x0a /* Public */,
      12,    1,   80,    2, 0x0a /* Public */,
      15,    1,   83,    2, 0x0a /* Public */,
      16,    1,   86,    2, 0x0a /* Public */,
      17,    1,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 13,   14,

       0        // eod
};

void PanelPluginsModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PanelPluginsModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pluginAdded((*reinterpret_cast< Plugin*(*)>(_a[1]))); break;
        case 1: _t->pluginRemoved((*reinterpret_cast< Plugin*(*)>(_a[1]))); break;
        case 2: _t->pluginMoved((*reinterpret_cast< Plugin*(*)>(_a[1]))); break;
        case 3: _t->pluginMovedUp((*reinterpret_cast< Plugin*(*)>(_a[1]))); break;
        case 4: _t->addPlugin((*reinterpret_cast< const UKUi::PluginInfo(*)>(_a[1]))); break;
        case 5: _t->removePlugin(); break;
        case 6: _t->onMovePluginUp((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: _t->onMovePluginDown((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: _t->onConfigurePlugin((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 9: _t->onRemovePlugin((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PanelPluginsModel::*)(Plugin * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PanelPluginsModel::pluginAdded)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PanelPluginsModel::*)(Plugin * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PanelPluginsModel::pluginRemoved)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PanelPluginsModel::*)(Plugin * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PanelPluginsModel::pluginMoved)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PanelPluginsModel::*)(Plugin * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PanelPluginsModel::pluginMovedUp)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PanelPluginsModel::staticMetaObject = { {
    &QAbstractListModel::staticMetaObject,
    qt_meta_stringdata_PanelPluginsModel.data,
    qt_meta_data_PanelPluginsModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PanelPluginsModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PanelPluginsModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PanelPluginsModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int PanelPluginsModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void PanelPluginsModel::pluginAdded(Plugin * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PanelPluginsModel::pluginRemoved(Plugin * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PanelPluginsModel::pluginMoved(Plugin * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PanelPluginsModel::pluginMovedUp(Plugin * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
