/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010- Facebook, Inc. (http://www.facebook.com)         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef __EVAL_ARRAY_EXPRESSION_H__
#define __EVAL_ARRAY_EXPRESSION_H__

#include <runtime/base/types.h>
#include <runtime/eval/ast/expression.h>

namespace HPHP {
namespace Eval {
///////////////////////////////////////////////////////////////////////////////

DECLARE_AST_PTR(ArrayPair);
DECLARE_AST_PTR(ArrayExpression);
DECLARE_AST_PTR(LvalExpression);

class ArrayPair : public Construct {
public:
  ArrayPair(CONSTRUCT_ARGS);
  ArrayPair(CONSTRUCT_ARGS, ExpressionPtr key);
  virtual ArrayPair *optimize(VariableEnvironment &env) = 0;
  virtual void set(VariableEnvironment &env, Array &arr) const = 0;
  virtual bool evalScalar(VariableEnvironment &env, Variant &r) const = 0;
  virtual bool checkStaticScalar(VariableEnvironment &env) const
    = 0;
protected:
  ExpressionPtr m_key;
  Variant key(VariableEnvironment &env) const;
};

class ArrayPairVal : public ArrayPair {
public:
  ArrayPairVal(CONSTRUCT_ARGS, ExpressionPtr key, ExpressionPtr val);
  ArrayPairVal(CONSTRUCT_ARGS, ExpressionPtr val);
  virtual void set(VariableEnvironment &env, Array &arr) const;
  virtual void dump(std::ostream &out) const;
  virtual ArrayPair *optimize(VariableEnvironment &env);
  virtual bool evalScalar(VariableEnvironment &env, Variant &r) const {
    return (!m_key || m_key->evalScalar(env, r)) && m_val->evalScalar(env, r);
  }
  virtual bool checkStaticScalar(VariableEnvironment &env) const {
    Variant r; 
    return (!m_key || m_key->evalStaticScalar(env, r)) &&
           m_val->evalStaticScalar(env, r);
  }
private:
  ExpressionPtr m_val;
  Variant val(VariableEnvironment &env) const;
};

class ArrayPairRef : public ArrayPair {
public:
  ArrayPairRef(CONSTRUCT_ARGS, ExpressionPtr key, LvalExpressionPtr val);
  ArrayPairRef(CONSTRUCT_ARGS, LvalExpressionPtr val);
  virtual void set(VariableEnvironment &env, Array &arr) const;
  virtual void dump(std::ostream &out) const;
  virtual ArrayPair *optimize(VariableEnvironment &env);
  virtual bool evalScalar(VariableEnvironment &env, Variant &r) const {
    return false;
  }
  virtual bool checkStaticScalar(VariableEnvironment &env) const {
    throw FatalErrorException("evalStaticScalar not implemented.");
  }
private:
  LvalExpressionPtr m_val;
  Variant &val(VariableEnvironment &env) const;
};

class ArrayExpression : public Expression {
public:
  ArrayExpression(EXPRESSION_ARGS, const std::vector<ArrayPairPtr> &elems);
  virtual Expression *optimize(VariableEnvironment &env);
  virtual Variant eval(VariableEnvironment &env) const;
  virtual bool evalScalar(VariableEnvironment &env, Variant &r) const;
  virtual bool evalStaticScalar(VariableEnvironment &env, Variant &r) const;
  virtual void dump(std::ostream &out) const;
private:
  std::vector<ArrayPairPtr> m_elems;
};

void optimize(VariableEnvironment &env, ArrayPairPtr &ap);

///////////////////////////////////////////////////////////////////////////////
}
}

#endif /* __EVAL_ARRAY_EXPRESSION_H__ */
