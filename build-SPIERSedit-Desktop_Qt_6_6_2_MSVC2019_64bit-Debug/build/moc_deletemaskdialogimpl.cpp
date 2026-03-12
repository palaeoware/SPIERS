/****************************************************************************
** Meta object code from reading C++ file 'deletemaskdialogimpl.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../SPIERSedit/src/deletemaskdialogimpl.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'deletemaskdialogimpl.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS = QtMocHelpers::stringData(
    "DeleteMaskDialogImpl",
    "on_ListTargetMasks_itemSelectionChanged",
    "",
    "on_buttonBox_accepted",
    "on_buttonBox_rejected",
    "Timer"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS_t {
    uint offsetsAndSizes[12];
    char stringdata0[21];
    char stringdata1[40];
    char stringdata2[1];
    char stringdata3[22];
    char stringdata4[22];
    char stringdata5[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS_t qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS = {
    {
        QT_MOC_LITERAL(0, 20),  // "DeleteMaskDialogImpl"
        QT_MOC_LITERAL(21, 39),  // "on_ListTargetMasks_itemSelect..."
        QT_MOC_LITERAL(61, 0),  // ""
        QT_MOC_LITERAL(62, 21),  // "on_buttonBox_accepted"
        QT_MOC_LITERAL(84, 21),  // "on_buttonBox_rejected"
        QT_MOC_LITERAL(106, 5)   // "Timer"
    },
    "DeleteMaskDialogImpl",
    "on_ListTargetMasks_itemSelectionChanged",
    "",
    "on_buttonBox_accepted",
    "on_buttonBox_rejected",
    "Timer"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDeleteMaskDialogImplENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x08,    1 /* Private */,
       3,    0,   39,    2, 0x08,    2 /* Private */,
       4,    0,   40,    2, 0x08,    3 /* Private */,
       5,    0,   41,    2, 0x08,    4 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject DeleteMaskDialogImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDeleteMaskDialogImplENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DeleteMaskDialogImpl, std::true_type>,
        // method 'on_ListTargetMasks_itemSelectionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_buttonBox_accepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_buttonBox_rejected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Timer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void DeleteMaskDialogImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DeleteMaskDialogImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_ListTargetMasks_itemSelectionChanged(); break;
        case 1: _t->on_buttonBox_accepted(); break;
        case 2: _t->on_buttonBox_rejected(); break;
        case 3: _t->Timer(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *DeleteMaskDialogImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeleteMaskDialogImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDeleteMaskDialogImplENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui::DeleteMaskDialog"))
        return static_cast< Ui::DeleteMaskDialog*>(this);
    return QDialog::qt_metacast(_clname);
}

int DeleteMaskDialogImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
