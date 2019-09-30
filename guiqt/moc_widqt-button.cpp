/****************************************************************************
** Meta object code from reading C++ file 'widqt-button.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../include/guiqt/widqt-button.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widqt-button.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TGUI_ButtonStd_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TGUI_ButtonStd_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TGUI_ButtonStd_t qt_meta_stringdata_TGUI_ButtonStd = {
    {
QT_MOC_LITERAL(0, 0, 14) // "TGUI_ButtonStd"

    },
    "TGUI_ButtonStd"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TGUI_ButtonStd[] = {

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

void TGUI_ButtonStd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject TGUI_ButtonStd::staticMetaObject = { {
    &QToolButton::staticMetaObject,
    qt_meta_stringdata_TGUI_ButtonStd.data,
    qt_meta_data_TGUI_ButtonStd,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TGUI_ButtonStd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TGUI_ButtonStd::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TGUI_ButtonStd.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "TGUI_Button"))
        return static_cast< TGUI_Button*>(this);
    return QToolButton::qt_metacast(_clname);
}

int TGUI_ButtonStd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_TGUI_ButtonOnOff_t {
    QByteArrayData data[5];
    char stringdata0[54];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TGUI_ButtonOnOff_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TGUI_ButtonOnOff_t qt_meta_stringdata_TGUI_ButtonOnOff = {
    {
QT_MOC_LITERAL(0, 0, 16), // "TGUI_ButtonOnOff"
QT_MOC_LITERAL(1, 17, 8), // "selected"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 7), // "checked"
QT_MOC_LITERAL(4, 35, 18) // "selectedByShortcut"

    },
    "TGUI_ButtonOnOff\0selected\0\0checked\0"
    "selectedByShortcut"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TGUI_ButtonOnOff[] = {

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
       1,    1,   24,    2, 0x0a /* Public */,
       4,    0,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

       0        // eod
};

void TGUI_ButtonOnOff::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TGUI_ButtonOnOff *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selected((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->selectedByShortcut(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TGUI_ButtonOnOff::staticMetaObject = { {
    &QToolButton::staticMetaObject,
    qt_meta_stringdata_TGUI_ButtonOnOff.data,
    qt_meta_data_TGUI_ButtonOnOff,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TGUI_ButtonOnOff::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TGUI_ButtonOnOff::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TGUI_ButtonOnOff.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "TGUI_Button"))
        return static_cast< TGUI_Button*>(this);
    return QToolButton::qt_metacast(_clname);
}

int TGUI_ButtonOnOff::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_TGUI_ButtonCB_t {
    QByteArrayData data[4];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TGUI_ButtonCB_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TGUI_ButtonCB_t qt_meta_stringdata_TGUI_ButtonCB = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TGUI_ButtonCB"
QT_MOC_LITERAL(1, 14, 12), // "stateChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 5) // "state"

    },
    "TGUI_ButtonCB\0stateChanged\0\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TGUI_ButtonCB[] = {

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
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void TGUI_ButtonCB::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TGUI_ButtonCB *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TGUI_ButtonCB::staticMetaObject = { {
    &QCheckBox::staticMetaObject,
    qt_meta_stringdata_TGUI_ButtonCB.data,
    qt_meta_data_TGUI_ButtonCB,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TGUI_ButtonCB::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TGUI_ButtonCB::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TGUI_ButtonCB.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "TGUI_Button"))
        return static_cast< TGUI_Button*>(this);
    return QCheckBox::qt_metacast(_clname);
}

int TGUI_ButtonCB::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCheckBox::qt_metacall(_c, _id, _a);
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
