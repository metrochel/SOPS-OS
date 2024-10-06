//
//  Управление объектами AML
//
//  - Перерабатывает AML-код.
//
#ifndef _AML_OBJS_INCL
#define _AML_OBJS_INCL

#include "../util/nums.hpp"
#include "../memmgr/memmgr.hpp"
#include "../util/util.hpp"
#include "../io/com.hpp"

#define ones defBlockRevision > 1 ? maxqword : maxdword
#define recret recDepth--; return

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

extern const dword varPathBase;
extern AMLName* varPath;

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

inline byte getVarPathLen() {
    return ((dword)varPath - varPathBase) / sizeof(AMLName);
}

inline void clearParsingPath() {
    parsingPath = (AMLName*)parsingPathBase;
    for (byte i = 0; i < 0x10; i++) {
        parsingPath[i] = 0;
    }
}

inline void clearVarPath() {
    varPath = (AMLName*)varPathBase;
    for (byte i = 0; i < 0x10; i++) {
        varPath[i] = 0;
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

inline void logVarPath() {
    logPath((AMLName*)varPathBase, getVarPathLen());
}

byte strToPath(const char* str, AMLName* out);

void storeParsingPath();

void restoreParsingPath();

qword getIntegerTerm(byte *aml);
byte getIntegerTermBytes(byte *aml);
byte getIntegerTermBytes(qword value);

byte encodeIntegerTerm(qword num, byte *ptr);

dword getPkgLength(byte *pkg);

inline byte getPkgBytes(dword value) {
    if (value < 64)
        return 1;
    if (value < 0x1000)
        return 2;
    if (value < 0x100000)
        return 3;
    return 4;
}

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

/// @brief Обрабатывает ThermalZone.
/// @param aml Указатель на AML-код
/// @return Длина ThermalZone в байтах
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

dword parseMutex(byte *aml);

/// @brief Обрабатывает TermList.
/// @param aml Указатель на AML-код
/// @param len Длина TermList в байтах
void parseTermList(byte *aml, dword len);

word getName(byte *aml);

dword nameACPIObj(byte* aml);

byte* getACPIObjAddr(AMLName* path, byte length);

byte* getACPIObjAddr(const char* path);

dword getACPIObjLen(const char* path);

dword getACPIObjLen(AMLName *path, byte length);

dword getACPIObjLen(byte *ptr);

void remapACPIObj(const char* path, byte *newAddr);

void remapACPIObj(AMLName *path, byte length, byte *newAddr);

bool sameACPIDomain(dword ptr1, dword ptr2);

// ### FuncFrame
// Структура, описывающая данный вызов функции.
struct FuncFrame {
    TermArg locals[7];      // Локальные переменные (Local0 - Local6)
    TermArg args[7];        // Аргументы функции (Arg0 - Arg6)
    byte syncLevel;         // Уровень синхронизации функции
    byte *addr;             // Адрес функции
    byte *tmpSpace;         // Адрес временного пространства функции
};

/// @brief Вызывает метод, определённый в AML.
/// @param path Путь к методу
/// @param ... Аргументы метода
/// @return Значение, возвращённое методом
qword callMethod(const char* path, ...);

/// @brief Исполняет метод.
/// @param code Указатель на код
/// @param length Длина кода в байтах
/// @param syncLevel Уровень синхронизации
/// @param argc Число аргументов
/// @param args Аргументы метода
/// @return Значение, возвращённое методом
TermArg runMethod(byte *code, dword length, byte syncLevel, FuncFrame *frame);

/// @brief Извлекает значение TermArg по данному адресу.
/// @param code Адрес извлечения
/// @param frame Структура вызова функции
/// @return Извлечённое значение
/// @note Если извлечение будет провалено, тип возвращаемого значения будет установлен на `maxqword`.
TermArg getTermArg(byte*& code, FuncFrame* frame);

/// @brief Устанавливает значение TermArg по данному адресу на другое.
/// @param code Адрес значения
/// @param frame Структура вызова функции
/// @param newVal Новое значение
void setTermArg(byte*& code, FuncFrame* frame, TermArg newVal);

void createBufferField(byte *buf, dword offset, dword size, byte *&dest);

dword upgradeBuffer(byte *buf, byte *&out, FuncFrame *frame);

dword upgradePackage(byte *pkg, byte *&out, FuncFrame *frame);

void processEvent(byte eventNo);

#endif