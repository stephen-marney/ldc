#ifndef LLVMDC_GEN_DVALUE_H
#define LLVMDC_GEN_DVALUE_H

/*
These classes are used for generating the IR. They encapsulate D values and
provide a common interface to the most common operations. When more specialized
handling is necessary, they hold enough information to do-the-right-thing (TM)
*/

#include <cassert>
#include "root.h"

struct Type;
struct Dsymbol;
struct VarDeclaration;
struct FuncDeclaration;

namespace llvm
{
    class Value;
    class Type;
    class Constant;
}

struct DImValue;
struct DConstValue;
struct DNullValue;
struct DVarValue;
struct DFieldValue;
struct DThisValue;
struct DFuncValue;
struct DSliceValue;
struct DArrayLenValue;
struct DLRValue;
struct DComplexValue;

// base class for d-values
struct DValue : Object
{
    virtual Type* getType() = 0;

    virtual LLValue* getLVal() { assert(0); return 0; }
    virtual LLValue* getRVal() { assert(0); return 0; }

    virtual bool isLVal() { return false; }

    virtual DImValue* isIm() { return NULL; }
    virtual DConstValue* isConst() { return NULL; }
    virtual DNullValue* isNull() { return NULL; }
    virtual DVarValue* isVar() { return NULL; }
    virtual DFieldValue* isField() { return NULL; }
    virtual DThisValue* isThis() { return NULL; }
    virtual DSliceValue* isSlice() { return NULL; }
    virtual DFuncValue* isFunc() { return NULL; }
    virtual DArrayLenValue* isArrayLen() { return NULL; }
    virtual DComplexValue* isComplex() { return NULL; }
    virtual DLRValue* isLRValue() { return NULL; }

    virtual bool inPlace() { return false; }

protected:
    DValue() {}
    DValue(const DValue&) { }
    DValue& operator=(const DValue&) { return *this; }
};

// immediate d-value
struct DImValue : DValue
{
    Type* type;
    LLValue* val;
    bool inplace;

    DImValue(Type* t, LLValue* v, bool in_place = false) { type = t; val = v; inplace = in_place; }

    virtual LLValue* getRVal() { assert(val); return val; }

    virtual Type* getType() { assert(type); return type; }
    virtual DImValue* isIm() { return this; }

    virtual bool inPlace() { return inplace; }
};

// constant d-value
struct DConstValue : DValue
{
    Type* type;
    LLConstant* c;

    DConstValue(Type* t, LLConstant* con) { type = t; c = con; }

    virtual LLValue* getRVal();

    virtual Type* getType() { assert(type); return type; }
    virtual DConstValue* isConst() { return this; }
};

// null d-value
struct DNullValue : DConstValue
{
    DNullValue(Type* t, LLConstant* con) : DConstValue(t,con) {}
    virtual DNullValue* isNull() { return this; }
};

// variable d-value
struct DVarValue : DValue
{
    Type* type;
    VarDeclaration* var;
    LLValue* val;
    LLValue* rval;
    bool lval;

    DVarValue(VarDeclaration* vd, LLValue* llvmValue, bool lvalue);
    DVarValue(Type* vd, LLValue* lv, LLValue* rv);
    DVarValue(Type* t, LLValue* llvmValue, bool lvalue);

    virtual bool isLVal() { return val && lval; }
    virtual LLValue* getLVal();
    virtual LLValue* getRVal();

    virtual Type* getType() { assert(type); return type; }
    virtual DVarValue* isVar() { return this; }
};

// field d-value
struct DFieldValue : DVarValue
{
    DFieldValue(Type* t, LLValue* llvmValue, bool l) : DVarValue(t, llvmValue, l) {}
    virtual DFieldValue* isField() { return this; }
};

// this d-value
struct DThisValue : DVarValue
{
    DThisValue(VarDeclaration* vd, LLValue* llvmValue) : DVarValue(vd, llvmValue, true) {}
    virtual DThisValue* isThis() { return this; }
};

// slice d-value
struct DSliceValue : DValue
{
    Type* type;
    LLValue* len;
    LLValue* ptr;

    DSliceValue(Type* t, LLValue* l, LLValue* p) { type=t; ptr=p; len=l; }

    virtual Type* getType() { assert(type); return type; }
    virtual DSliceValue* isSlice() { return this; }
};

// function d-value
struct DFuncValue : DValue
{
    Type* type;
    FuncDeclaration* func;
    LLValue* val;
    LLValue* vthis;

    DFuncValue(FuncDeclaration* fd, LLValue* v, LLValue* vt = 0);

    virtual LLValue* getRVal();

    virtual Type* getType() { assert(type); return type; }
    virtual DFuncValue* isFunc() { return this; }
};

// l-value and r-value pair d-value
struct DLRValue : DValue
{
    Type* ltype;
    LLValue* lval;
    Type* rtype;
    LLValue* rval;

    DLRValue(Type* lt, LLValue* l, Type* rt, LLValue* r) {
        ltype = lt;
        lval = l;
        rtype = rt;
        rval = r;
    }

    virtual bool isLVal() { return lval; }
    virtual LLValue* getLVal() { assert(lval); return lval; }
    virtual LLValue* getRVal() { assert(rval); return rval; }

    Type* getLType() { return ltype; }
    Type* getRType() { return rtype; }
    virtual Type* getType() { return getRType(); }
    virtual DLRValue* isLRValue() { return this; }
};

// complex number immediate d-value (much like slice)
struct DComplexValue : DValue
{
    Type* type;
    LLValue* re;
    LLValue* im;

    DComplexValue(Type* t, LLValue* r, LLValue* i) {
        type = t;
        re = r;
        im = i;
    }

    virtual Type* getType() { assert(type); return type; }
    virtual DComplexValue* isComplex() { return this; }
};

#endif // LLVMDC_GEN_DVALUE_H
