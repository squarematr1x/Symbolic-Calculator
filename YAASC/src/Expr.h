#pragma once

#include <iostream>

#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

enum class ExprType
{
	MUL,
	DIV,
	ADD,
	POW,
	FAC,
	LOG,
	SIN,
	COS,
	TAN,
	DERIVATIVE,
	INTEGRAL,
	INTEGER,
	VARIABLE,
	SUB,
	FLOAT,
	FRACTION,
	PI,
	e,
	NIL
};

class Expr
{
private:
	std::unique_ptr<Expr> m_left;
	std::unique_ptr<Expr> m_right;

public:
	Expr(std::unique_ptr<Expr> left = nullptr, std::unique_ptr<Expr> right = nullptr)
		: m_left(std::move(left)), m_right(std::move(right))
	{
	}

	virtual ~Expr()
	{
	}

	virtual int Eval(std::map<std::string, int> env) = 0;
	virtual std::string Name() const = 0;

	virtual int ChildrenSize() const { return 0; }
	virtual ExprType ExpressionType() const { return ExprType::NIL; }

	virtual bool IsTerminal() const { return false; }
	virtual bool IsFunc() const { return false; }
	virtual bool IsFac() const { return false; }
	virtual bool IsLog() const { return false; }
	virtual bool IsLn() const { return false; }
	virtual bool IsSin() const { return false;}
	virtual bool IsCos() const { return false; }
	virtual bool IsTan() const { return false; }
	virtual bool IsDerivative() const{ return false; }
	virtual bool IsIntegral() const { return false; }
	virtual bool IsAssociative() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual bool IsInteger() const { return false; }
	virtual bool IsFloat() const { return false; }
	virtual bool IsNumber() const { return false; }
	virtual bool IsMul() const { return false; }
	virtual bool IsAdd() const { return false; }
	virtual bool IsPow() const { return false; }
	virtual bool IsSpecial() const { return false; }
	virtual bool IsPi() const { return false; }
	virtual bool IsE() const { return false; }
	virtual bool IsGeneric() { return false; }
	virtual bool IsZero() { return false; }
	virtual bool IsOne() { return false; }

	virtual bool HasLeftChild();
	virtual bool HasRightChild();
	virtual bool HasChildren();
	virtual bool HasNoChildren();
	virtual bool HasAssociativeChildren();
	virtual bool HasNumberChildren();
	virtual bool HasMulChildren();
	virtual bool HasPowChildren();
	virtual bool HasFloatChild();
	virtual bool LeftIsTerminal();
	virtual bool RightIsTerminal();

	virtual std::unique_ptr<Expr>& Left() { return m_left; }
	virtual std::unique_ptr<Expr>& Right(){ return m_right; }
	virtual std::unique_ptr<Expr>& ChildAt(int i) { (void)i; return m_left; }
	virtual std::unique_ptr<Expr>& Param() { return m_left; }
	virtual std::unique_ptr<Expr>& Base() { return m_left; }

	virtual void SwapChildren();
	virtual void SortChildren() {}
	virtual void SortAddChildren() {}
	virtual void SortMulChildren() {}
	virtual void ClearChildren() {}
	virtual void RemoveChildren(int from, int to) { (void)from; (void)to; }
	virtual void RemoveChild(int i) { (void)i; }
	virtual void AddChild(std::unique_ptr<Expr> child) { (void)child; }
	virtual void SetChildAt(int i, std::unique_ptr<Expr> child) { (void)i; }
	virtual void SetLeft(std::unique_ptr<Expr> expr) { m_left = std::move(expr); }
	virtual void SetRight(std::unique_ptr<Expr> expr) { m_right = std::move(expr); }
	virtual void SetBase(std::unique_ptr<Expr> expr) {}

	friend std::ostream& operator<< (std::ostream& out, const std::unique_ptr<Expr>& expr);

	friend bool operator== (const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);
	friend bool operator!= (const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);

	friend bool SameExpressions(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b);
	friend void CheckExpressions(const std::unique_ptr<Expr>& expr_a, const std::unique_ptr<Expr>& expr_b, bool& same);

	friend const std::unique_ptr<Expr>& LeftmostChild(const std::unique_ptr<Expr>& expr);
	friend const std::unique_ptr<Expr>& LeftmostAssociativeOperator(const std::unique_ptr<Expr>& expr);
};

template <typename T>
class Atomic : public Expr
{
protected:
	T m_atom;

public:
	Atomic(T atom) : m_atom(atom)
	{
	}

	virtual ~Atomic()
	{
	}

	bool IsTerminal() const { return true; }
};

class Integer : public Atomic<int>
{
public:
	Integer(int value)
		: Atomic(value)
	{
	}

	int Eval(std::map<std::string, int> env)
	{
		(void)env;
		return m_atom;
	}

	std::string Name() const { return std::to_string(m_atom); }
	ExprType ExpressionType() const { return ExprType::INTEGER; }
	bool IsInteger() const { return true; }
	bool IsNumber() const { return true; }
	bool IsZero();
	bool IsOne();
};

class Float : public Atomic<float>
{
public:
	Float(float value)
		: Atomic(value)
	{
	}

	int Eval(std::map<std::string, int> env)
	{
		(void)env;
		return (int)m_atom;
	}

	std::string Name() const;
	ExprType ExpressionType() const { return ExprType::FLOAT; }
	bool IsFloat() const { return true; }
	bool IsNumber() const { return true; }
	bool IsZero();
	bool IsOne();
};

class Var : public Atomic<std::string>
{
public:
	Var(std::string name)
		: Atomic(name)
	{
	}

	virtual ~Var()
	{
	}

	int Eval(std::map<std::string, int> env) { return env.at(m_atom); }
	std::string Name() const { return m_atom; }
	ExprType ExpressionType() const { return ExprType::VARIABLE; }
	bool IsVar() const { return true; }
};

class Special : public Var
{
public:
	Special(std::string name)
		: Var(name)
	{
	}

	virtual ~Special()
	{
	}

	bool IsSpecial() const { return true; }
};

class Pi : public Special
{
public:
	Pi() : Special("PI")
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	ExprType ExpressionType() const { return ExprType::PI; }
	bool IsPi() const { return true; }
};

class E : public Special
{
public:
	E() : Special("e")
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	ExprType ExpressionType() const { return ExprType::e; }
	bool IsE() const { return true; }
};

class Func : public Expr
{
protected: 
	std::unique_ptr<Expr> m_param;

public: 
	Func(std::unique_ptr<Expr> param)
		: Expr(nullptr, nullptr), m_param(std::move(param))
	{
	}

	virtual ~Func()
	{
	}

	std::unique_ptr<Expr>& Param() { return m_param; }
	bool IsFunc() const { return true; }
};

class Fac : public Func
{
public:
	Fac(std::unique_ptr<Expr> param)
		: Func(std::move(param))
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	bool IsFac() const { return true; }
	std::string Name() const { return "!"; }
};

class Log : public Func
{
protected:
	std::unique_ptr<Expr> m_base;

public:
	Log(std::unique_ptr<Expr> param, std::unique_ptr<Expr> base)
		: m_base(std::move(base)), Func(std::move(param))
	{
	}

	virtual ~Log()
	{
	}

	void SetBase(std::unique_ptr<Expr> expr) { m_base = std::move(expr); }
	int Eval(std::map<std::string, int> env) { return 0; }
	std::unique_ptr<Expr>& Base() { return m_base; }
	bool IsLog() const { return true; }
	std::string Name() const { return "log"; }
};

class Ln : public Log
{
public:
	Ln(std::unique_ptr<Expr> param)
		: Log(std::move(param), std::move(std::make_unique<E>()))
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	bool IsLn() const { return true; }
	std::string Name() const { return "ln"; }
};

class Sin : public Func
{
public:
	Sin(std::unique_ptr<Expr> param)
		: Func(std::move(param))
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	bool IsSin() const { return true; }
	std::string Name() const { return "sin"; }
};

class Cos : public Func
{
public:
	Cos(std::unique_ptr<Expr> param)
		: Func(std::move(param))
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	bool IsCos() const { return true; }
	std::string Name() const { return "cos"; }
};

class Tan : public Func
{
public:
	Tan(std::unique_ptr<Expr> param)
		: Func(std::move(param))
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	bool IsTan() const { return true; }
	std::string Name() const { return "tan"; }
};

class Derivative : public Func
{
private:
	std::string m_respect_to;

public:
	Derivative(std::unique_ptr<Expr> param, std::string respect_to)
		: m_respect_to(respect_to), Func(std::move(param))
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	bool IsDerivative() const { return true; }
	std::string Name() const { return "D"; }
};

class Integral : public Func
{
private:
	std::string m_respect_to;

public:
	Integral(std::unique_ptr<Expr> param, std::string respect_to)
		: m_respect_to(respect_to), Func(std::move(param))
	{
	}

	int Eval(std::map<std::string, int> env) { return 0; }
	bool IsIntegral() const { return true; }
	std::string Name() const { return "I"; }
};

class Associative : public Expr
{
protected:
	std::vector<std::unique_ptr<Expr>> m_children;

public:
	Associative(std::unique_ptr<Expr> left = nullptr, std::unique_ptr<Expr> right = nullptr)
		: Expr(std::move(left), std::move(right))
	{
	}

	virtual ~Associative()
	{
	}

	void ClearChildren() { m_children.clear(); }
	void SortChildren();
	void SortAddChildren();
	void SortMulChildren();
	void RemoveChildren(int from, int to);
	void RemoveChild(int i);
	void AddChild(std::unique_ptr<Expr> expr) { m_children.push_back(std::move(expr)); }
	void SetChildAt(int i, std::unique_ptr<Expr> child);

	int ChildrenSize() const { return (int)m_children.size(); }

	std::unique_ptr<Expr>& ChildAt(int i);

	bool IsAssociative() const { return true; }
	bool IsGeneric();
};

class Mul : public Associative
{
public:
	Mul(std::unique_ptr<Expr> left = nullptr, std::unique_ptr<Expr> right = nullptr)
		: Associative(std::move(left), std::move(right))
	{
	}

	int Eval(std::map<std::string, int> env){ return Left()->Eval(env) * Right()->Eval(env); }
	std::string Name() const { return "*"; }
	ExprType ExpressionType() const { return ExprType::MUL; }
	bool IsMul() const { return true; }
};

class Add : public Associative
{
public:
	Add(std::unique_ptr<Expr> left = nullptr, std::unique_ptr<Expr> right = nullptr)
		: Associative(std::move(left), std::move(right))
	{
	}

	int Eval(std::map<std::string, int> env){ return Left()->Eval(env) + Right()->Eval(env); }
	std::string Name() const { return "+"; }
	ExprType ExpressionType() const { return ExprType::ADD; }
	bool IsAdd() const { return true; }
};

class Pow : public Expr
{
public:
	Pow(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
		: Expr(std::move(left), std::move(right))
	{
	}

	int Eval(std::map<std::string, int> env){ return (int)std::pow(Left()->Eval(env), Right()->Eval(env)); }
	std::string Name() const { return "^"; }
	ExprType ExpressionType() const { return ExprType::POW; }
	bool IsPow() const { return true; }
};
