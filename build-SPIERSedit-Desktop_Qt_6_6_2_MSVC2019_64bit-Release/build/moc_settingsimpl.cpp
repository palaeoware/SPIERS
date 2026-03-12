/****************************************************************************
** Meta object code from reading C++ file 'settingsimpl.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SPIERSedit/src/settingsimpl.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settingsimpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSSettingsImplENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSSettingsImplENDCLASS = QtMocHelpers::stringData(
    "SettingsImpl",
    "on_RecompressSourceFiles_pressed",
    "",
    "on_RecompressFiles_pressed",
    "on_SliderFileCompression_valueChanged",
    "value",
    "on_SliderCacheCompression_valueChanged",
    "on_buttonBox_accepted",
    "on_buttonBox_rejected",
    "on_pushButton_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSSettingsImplENDCLASS_t {
    uint offsetsAndSizes[20];
    char stringdata0[13];
    char stringdata1[33];
    char stringdata2[1];
    char stringdata3[27];
    char stringdata4[38];
    char stringdata5[6];
    char stringdata6[39];
    char stringdata7[22];
    char stringdata8[22];
    char stringdata9[22];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSSettingsImplENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSSettingsImplENDCLASS_t qt_meta_stringdata_CLASSSettingsImplENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "SettingsImpl"
        QT_MOC_LITERAL(13, 32),  // "on_RecompressSourceFiles_pressed"
        QT_MOC_LITERAL(46, 0),  // ""
        QT_MOC_LITERAL(47, 26),  // "on_RecompressFiles_pressed"
        QT_MOC_LITERAL(74, 37),  // "on_SliderFileCompression_valu..."
        QT_MOC_LITERAL(112, 5),  // "value"
        QT_MOC_LITERAL(118, 38),  // "on_SliderCacheCompression_val..."
        QT_MOC_LITERAL(157, 21),  // "on_buttonBox_accepted"
        QT_MOC_LITERAL(179, 21),  // "on_buttonBox_rejected"
        QT_MOC_LITERAL(201, 21)   // "on_pushButton_clicked"
    },
    "SettingsImpl",
    "on_RecompressSourceFiles_pressed",
    "",
    "on_RecompressFiles_pressed",
    "on_SliderFileCompression_valueChanged",
    "value",
    "on_SliderCacheCompression_valueChanged",
    "on_buttonBox_accepted",
    "on_buttonBox_rejected",
    "on_pushButton_clicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSettingsImplENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    0,   57,    2, 0x08,    2 /* Private */,
       4,    1,   58,    2, 0x08,    3 /* Private */,
       6,    1,   61,    2, 0x08,    5 /* Private */,
       7,    0,   64,    2, 0x08,    7 /* Private */,
       8,    0,   65,    2, 0x08,    8 /* Private */,
       9,    0,   66,    2, 0x08,    9 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject SettingsImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSSettingsImplENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSettingsImplENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSettingsImplENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SettingsImpl, std::true_type>,
        // method 'on_RecompressSourceFiles_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_RecompressFiles_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_SliderFileCompression_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_SliderCacheCompression_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_buttonBox_accepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_buttonBox_rejected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void SettingsImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SettingsImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_RecompressSourceFiles_pressed(); break;
        case 1: _t->on_RecompressFiles_pressed(); break;
        case 2: _t->on_SliderFileCompression_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->on_SliderCacheCompression_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->on_buttonBox_accepted(); break;
        case 5: _t->on_buttonBox_rejected(); break;
        case 6: _t->on_pushButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject *SettingsImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SettingsImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSettingsImplENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::Settings"))
        return static_cast< Ui::Settings*>(this);
    return QDialog::qt_metacast(_clname);
}

int SettingsImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
