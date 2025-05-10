//
//	decl.h - Вспомогательный файл
//
//	Предоставляет пару вспомогательных макросов.
//

// BEGIN_DECLS и END_DECLS позволяют заголовкам быть совместимыми с C++.
// В каждом файле должен быть ровно один BEGIN_DECLS и ровно один END_DECLS.

#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS }
#else
#define BEGIN_DECLS
#define END_DECLS
#endif

