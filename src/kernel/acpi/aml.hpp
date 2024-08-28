//
//  Управление объектами AML
//
//  - Перерабатывает AML-код.
//
#ifndef _AML_OBJS_INCL
#define _AML_OBJS_INCL

#include <stdarg.h>
#include "../util/nums.hpp"
#include "../io/com.hpp"
#include "../memmgr/memmgr.hpp"
#include "../util/util.hpp"
#include "../util/nums.hpp"

typedef unsigned int AMLName;

// ### EISA ID
// EISA-идентификатор устройства.
struct EISAId {
    char mfg[3];            // Идентификатор
    byte prodNo[4];      // Код устройства
    byte reserved = 0;   // (резервирован для сохранения ровнения)
} __attribute__((packed));

// ### TermArg
// Обычный TermArg.
struct TermArg {
    qword type;         // Тип значения        
    qword value;        // Значение
} __attribute__((packed));

extern const dword parsingPathBase;
extern AMLName* parsingPath;

extern const dword scopePathBase;
extern AMLName* scopePath;

extern const dword bufferedPathBase;
extern AMLName* bufferedPath;

extern const dword acpiNamespaceBase;
extern dword* acpiNamespace;

extern const dword acpiFieldsBase;
extern byte* acpiFields;

extern const dword acpiDataBase;
extern byte* acpiData;

extern const dword acpiFuncsBase;
extern byte* acpiFuncs;

extern bool acpiNamespaceInit;

extern byte defBlockRevision;

inline byte getParsingPathLen() {
    return ((dword)parsingPath - parsingPathBase) / sizeof(AMLName);
}

inline byte getBufferedPathLen() {
    return ((dword)bufferedPath - bufferedPathBase) / sizeof(AMLName);
}

inline void clearParsingPath() {
    parsingPath = (AMLName*)parsingPathBase;
    for (byte i = 0; i < 0x10; i++) {
        parsingPath[i] = 0;
    }
}

inline bool isLeadNameChar(byte c) {
    return (c >= 'A' && c <= 'Z') || c == '_';
}

inline void logName(AMLName name) {
    byte *ptr = (byte*)(&(name));
    for (byte i = 0; i < 4; i++) {
        if (ptr[i] != '_' || i == 0)
            kdebug(ptr[i]);
    }
}

void logPath(AMLName* path, byte len);

inline void logParsingPath() {
    logPath((AMLName*)parsingPathBase, getParsingPathLen());
}

inline void storeParsingPath() {
    for (byte i = 0; i < getParsingPathLen(); i++) {
        *bufferedPath++ = *(AMLName*)(parsingPathBase + i * 4);
    }
    parsingPath = (AMLName*)parsingPathBase;
}

inline void restoreParsingPath() {
    byte len = getBufferedPathLen();
    bufferedPath = (AMLName*)bufferedPathBase;
    parsingPath = (AMLName*)parsingPathBase;
    for (byte i = 0; i < len; i++) {
        *parsingPath++ = bufferedPath[i];
    }
}

qword getIntegerTerm(byte *aml);
byte getIntegerTermBytes(byte *aml);
byte getIntegerTermBytes(qword value);

byte encodeIntegerTerm(qword num, byte *ptr);

dword getPkgLength(byte *pkg);

byte encodePkgLength(dword length, byte *out);

/// @brief Обрабатывает Definition Block.
/// @param aml Указатель на AML-код
void parseDefBlock(byte *aml);

/// @brief Обрабатывает Scope.
/// @param aml Указатель на AML-код
/// @return Длина Scope в байтах
dword parseScope(byte *aml);

/// @brief Обрабатывает Device.
/// @param aml Указатель на AML-код
/// @return Длина Device в байтах
dword parseDevice(byte *aml);

/// @brief Обрабатывает OperationRegion.
/// @param aml Указатель на AML-код
/// @return Длина AML-кода OperationRegion
dword parseOpRegion(byte *aml);

/// @brief Обрабатывает имя.
/// @param aml Указатель на AML-код
/// @returns Высший байт - длина имени в байтах, низший - длина имени в сегментах
word parseName(byte *aml);

/// @brief Обрабатывает Field.
/// @param aml Указатель на AML-код
/// @return Длина Field в байтах
dword parseField(byte *aml);

/// @brief Обрабатывает Method.
/// @param aml Указатель на AML-код
/// @return Длина Method в байтах
dword parseMethod(byte *aml);

/// @brief Обрабатывает Buffer.
/// @param aml Указатель на AML-код
/// @return Длина Buffer в байтах
dword parseBuffer(byte *aml);

/// @brief Обрабатывает Package.
/// @param aml Указатель на AML-код
/// @return Длина Package в байтах
dword parsePackage(byte *aml);

/// @brief Обрабатывает VarPackage.
/// @param aml Указатель на AML-код
/// @return Длина VarPackage в байтах
dword parseVarPackage(byte *aml);

/// @brief Обрабатывает TermList.
/// @param aml Указатель на AML-код
/// @param len Длина TermList в байтах
void parseTermList(byte *aml, dword len);

dword nameACPIObj(byte* aml);

byte* getACPIObjAddr(AMLName* path, byte length);

byte* getACPIObjAddr(const char* path);

dword getACPIObjLen(const char* path);

dword getACPIObjLen(AMLName *path, byte length);

dword getACPIObjLen(byte *ptr);

void remapACPIObj(const char* path, byte *newAddr);

void remapACPIObj(AMLName *path, byte length, byte *newAddr);

/// @brief Вызывает метод, определённый в AML.
/// @param path Путь к методу
/// @param ... Параметры метода
/// @return Значение, возвращённое методом
qword callMethod(const char* path, ...);

/// @brief Исполняет метод.
/// @param code Указатель на код
/// @param length Длина кода в байтах
/// @param args Аргументы метода
/// @return Значение, возвращённое методом
qword runMethod(byte *code, dword length, byte argc, va_list args);

TermArg getTermArg(byte*& code, TermArg*& locals, TermArg*& args);

void setTermArg(byte*& code, TermArg*& locals, TermArg*& args, TermArg newVal);

#endif