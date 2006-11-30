/*
 * ctable.h - include file for ctables
 *
 * $Id$
 *
 */

#include <tcl.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>

#include <sys/limits.h>

#ifdef WITH_PGTCL
#include <libpq-fe.h>
#endif

// define ctable search comparison types
#define CTABLE_COMP_FALSE 0
#define CTABLE_COMP_TRUE 1
#define CTABLE_COMP_NULL 2
#define CTABLE_COMP_NOTNULL 3
#define CTABLE_COMP_LT 4
#define CTABLE_COMP_LE 5
#define CTABLE_COMP_EQ 6
#define CTABLE_COMP_NE 7
#define CTABLE_COMP_GE 8
#define CTABLE_COMP_GT 9
#define CTABLE_COMP_MATCH 10
#define CTABLE_COMP_MATCH_CASE 11

// ctable sort struct - this controls everything about a sort
struct ctableSortStruct {
    int nFields;
    int *fields;
    int *directions;
};

#define CTABLE_STRING_MATCH_ANCHORED 0
#define CTABLE_STRING_MATCH_UNANCHORED 1
#define CTABLE_STRING_MATCH_PATTERN 2

struct ctableSearchMatchStruct {
    int             type;
    int             nocase;

    // boyer-moore stuff
    int            *skip;
    int             occ[UCHAR_MAX+1];
    int             nlen;
    unsigned char  *needle;
};

// ctable search component struct - one for each search expression in a
// ctable search
struct ctableSearchComponentStruct {
    int             fieldID;
    int             comparisonType;
    Tcl_Obj        *comparedToObject;
    char           *comparedToString;
    int             comparedToStringLength;
    void           *clientData;
};

// ctable search struct - this controls everything about a search
struct ctableSearchStruct {
    struct ctableTable                  *ctable;
    int                                  nComponents;
    struct ctableSearchComponentStruct  *components;

    int                                  countOnly;
    int                                  countMax;
    int                                  offset;
    int                                  limit;

    char                                *pattern;

    struct ctableSortStruct              sortControl;

    int                                 *retrieveFields;
    int                                  nRetrieveFields;

    int                                  noKeys;
    Tcl_Obj                             *codeBody;
    Tcl_Obj                             *varNameObj;
    Tcl_Obj                             *keyVarNameObj;
    int                                  useArrayGet;
    int                                  useArrayGetWithNulls;
    int                                  useGet;

    Tcl_Channel                          tabsepChannel;
    int                                  writingTabsep;
    int                                  writingTabsepIncludeFieldNames;
};

struct ctableCreatorTable {
    Tcl_HashTable     *registeredProcTablePtr;
    long unsigned int  nextAutoCounter;
    CONST char       **fieldNames;
    Tcl_Obj          **nameObjList;

    int                nFields;
    int               *fieldList;
    enum ctable_types *fieldTypes;
    int               *fieldsThatNeedQuoting;

    int (*search_compare) (Tcl_Interp *interp, struct ctableSearchStruct *searchControl, Tcl_HashEntry *hashEntryPtr);
    int (*sort_compare) (void *clientData, const void *hashEntryPtr1, const void *hashEntryPtr2);
    Tcl_Obj *(*get_field_obj) (Tcl_Interp *interp, void *pointer, int field);
    void (*dstring_append_get_tabsep) (char *key, void *pointer, int *fieldNums, int nFields, Tcl_DString *dsPtr, int noKey);
    Tcl_Obj *(*gen_list) (Tcl_Interp *interp, void *pointer);
    Tcl_Obj *(*gen_keyvalue_list) (Tcl_Interp *interp, void *pointer);
    Tcl_Obj *(*gen_nonnull_keyvalue_list) (Tcl_Interp *interp, void *pointer);
    int (*lappend_field) (Tcl_Interp *interp, Tcl_Obj *destListObj, void *p, int field);
    int (*lappend_field_and_name) (Tcl_Interp *interp, Tcl_Obj *destListObj, void *p, int field);
    int (*lappend_nonnull_field_and_name) (Tcl_Interp *interp, Tcl_Obj *destListObj, void *p, int field);
};

struct ctableTable {
    struct ctableCreatorTable *creatorTable;
    Tcl_HashTable             *keyTablePtr;
    Tcl_Command                commandInfo;
    long                       count;
};


// extern int ctable_SetupAndPerformSearch (Tcl_Interp *interp, Tcl_Obj *CONST objv[], int objc, struct ctableTable *ctable);

