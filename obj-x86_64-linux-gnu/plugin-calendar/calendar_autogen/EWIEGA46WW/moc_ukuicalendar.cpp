/****************************************************************************
** Meta object code from reading C++ file 'ukuicalendar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../plugin-calendar/ukuicalendar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ukuicalendar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_IndicatorCalendar_t {
    QByteArrayData data[8];
    char stringdata0[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IndicatorCalendar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IndicatorCalendar_t qt_meta_stringdata_IndicatorCalendar = {
    {
QT_MOC_LITERAL(0, 0, 17), // "IndicatorCalendar"
QT_MOC_LITERAL(1, 18, 11), // "deactivated"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 7), // "timeout"
QT_MOC_LITERAL(4, 39, 13), // "wheelScrolled"
QT_MOC_LITERAL(5, 53, 11), // "deletePopup"
QT_MOC_LITERAL(6, 65, 14), // "updateTimeText"
QT_MOC_LITERAL(7, 80, 11) // "hidewebview"

    },
    "IndicatorCalendar\0deactivated\0\0timeout\0"
    "wheelScrolled\0deletePopup\0updateTimeText\0"
    "hidewebview"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IndicatorCalendar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   45,    2, 0x08 /* Private */,
       4,    1,   46,    2, 0x08 /* Private */,
       5,    0,   49,    2, 0x08 /* Private */,
       6,    0,   50,    2, 0x08 /* Private */,
       7,    0,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void IndicatorCalendar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<IndicatorCalendar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deactivated(); break;
        case 1: _t->timeout(); break;
        case 2: _t->wheelScrolled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->deletePopup(); break;
        case 4: _t->updateTimeText(); break;
        case 5: _t->hidewebview(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (IndicatorCalendar::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&IndicatorCalendar::deactivated)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject IndicatorCalendar::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_IndicatorCalendar.data,
    qt_meta_data_IndicatorCalendar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *IndicatorCalendar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IndicatorCalendar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_IndicatorCalendar.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IUKUIPanelPlugin"))
        return static_cast< IUKUIPanelPlugin*>(this);
    return QWidget::qt_metacast(_clname);
}

int IndicatorCalendar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void IndicatorCalendar::deactivated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_CalendarActiveLabel_t {
    QByteArrayData data[5];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CalendarActiveLabel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CalendarActiveLabel_t qt_meta_stringdata_CalendarActiveLabel = {
    {
QT_MOC_LITERAL(0, 0, 19), // "CalendarActiveLabel"
QT_MOC_LITERAL(1, 20, 13), // "wheelScrolled"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 22), // "leftMouseButtonClicked"
QT_MOC_LITERAL(4, 58, 24) // "middleMouseButtonClicked"

    },
    "CalendarActiveLabel\0wheelScrolled\0\0"
    "leftMouseButtonClicked\0middleMouseButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CalendarActiveLabel[] = {

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
       1,    1,   29,    2, 0x06 /* Public */,
       3,    0,   32,    2, 0x06 /* Public */,
       4,    0,   33,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CalendarActiveLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CalendarActiveLabel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->wheelScrolled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->leftMouseButtonClicked(); break;
        case 2: _t->middleMouseButtonClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CalendarActiveLabel::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalendarActiveLabel::wheelScrolled)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CalendarActiveLabel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalendarActiveLabel::leftMouseButtonClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CalendarActiveLabel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CalendarActiveLabel::middleMouseButtonClicked)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CalendarActiveLabel::staticMetaObject = { {
    &QLabel::staticMetaObject,
    qt_meta_stringdata_CalendarActiveLabel.data,
    qt_meta_data_CalendarActiveLabel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CalendarActiveLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalendarActiveLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CalendarActiveLabel.stringdata0))
        return static_cast<void*>(this);
    return QLabel::qt_metacast(_clname);
}

int CalendarActiveLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
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
void CalendarActiveLabel::wheelScrolled(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CalendarActiveLabel::leftMouseButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CalendarActiveLabel::middleMouseButtonClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_UKUICalendarPluginLibrary_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UKUICalendarPluginLibrary_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UKUICalendarPluginLibrary_t qt_meta_stringdata_UKUICalendarPluginLibrary = {
    {
QT_MOC_LITERAL(0, 0, 25) // "UKUICalendarPluginLibrary"

    },
    "UKUICalendarPluginLibrary"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UKUICalendarPluginLibrary[] = {

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

void UKUICalendarPluginLibrary::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject UKUICalendarPluginLibrary::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_UKUICalendarPluginLibrary.data,
    qt_meta_data_UKUICalendarPluginLibrary,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UKUICalendarPluginLibrary::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UKUICalendarPluginLibrary::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UKUICalendarPluginLibrary.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IUKUIPanelPluginLibrary"))
        return static_cast< IUKUIPanelPluginLibrary*>(this);
    if (!strcmp(_clname, "ukui.org/Panel/PluginInterface/3.0"))
        return static_cast< IUKUIPanelPluginLibrary*>(this);
    return QObject::qt_metacast(_clname);
}

int UKUICalendarPluginLibrary::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}

QT_PLUGIN_METADATA_SECTION
static constexpr unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x22,  'u',  'k',  'u',  'i',  '.', 
    'o',  'r',  'g',  '/',  'P',  'a',  'n',  'e', 
    'l',  '/',  'P',  'l',  'u',  'g',  'i',  'n', 
    'I',  'n',  't',  'e',  'r',  'f',  'a',  'c', 
    'e',  '/',  '3',  '.',  '0', 
    // "className"
    0x03,  0x78,  0x19,  'U',  'K',  'U',  'I',  'C', 
    'a',  'l',  'e',  'n',  'd',  'a',  'r',  'P', 
    'l',  'u',  'g',  'i',  'n',  'L',  'i',  'b', 
    'r',  'a',  'r',  'y', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(UKUICalendarPluginLibrary, UKUICalendarPluginLibrary)

QT_WARNING_POP
QT_END_MOC_NAMESPACE
