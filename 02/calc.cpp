#include <iostream>
#include <string>
#include <stdexcept>

enum class Operation
{
	NOP=0,
	ADD,
	SUB,
	MUL,
	DIV,
};

class Expression
{
	std::int64_t value = 0;
	std::int64_t result;
	Operation op = Operation::NOP;
	Expression* left;
	Expression* right;
	std::string SkipSpaces(const std::string& exp)
	{
		std::string out;
		for (size_t i = 0; i < exp.size(); ++i)
			if (exp[i] != ' ') out += exp[i];
		return out;
	}
	std::string deletedoublemines(const std::string& exp)
	{
		std::string out;
		for (size_t i = 0; i < exp.size(); ++i)
		{
			if (exp[i] == '-')
			{
				if (exp[i + 1] == '-')
				{
					out += '+';
					++i;
				}
				else
					out += exp[i];
			}
			else if (exp[i] == '+')
			{
				if (exp[i + 1] == '-')
				{
					out += '-';
					++i;
				}
				else out += exp[i];
			}
			else
			out += exp[i];
		}
		return out;
	}
	std::int64_t Calculate(Expression* exp)
	{
		if (exp->op == Operation::NOP)
			return exp->value;
		Calculate(exp->left);
		Calculate(exp->right);
		switch (exp->op)
		{
		case Operation::ADD:
			exp->value = exp->left->value + exp->right->value;
			break;
		case Operation::SUB:
			exp->value = exp->left->value - exp->right->value;
			break;
		case Operation::MUL:
			exp->value = exp->left->value * exp->right->value;
			break;
		case Operation::DIV:
			if (exp->right->value == 0) throw std::runtime_error("");
			exp->value = exp->left->value / exp->right->value;
			break;
		case Operation::NOP:
			break;
		}

		return exp->value;
	}
	Expression* ParseAddSub(std::string &str)
	{
		Expression* _left = ParseMulDiv(str);
		while (true)
		{
			Operation op = Operation::NOP;
			std::string remainingStr = str;
			if (!ParseOperator(remainingStr, op) || (op != Operation::ADD && op != Operation::SUB))
				return _left;
			switch (op)
			{
			case Operation::ADD:
			case Operation::SUB:
				break;
			default:
				return _left;
			}
			str = remainingStr;
			Expression* _right = ParseMulDiv(str);
			Expression* expr=new Expression;
			expr->left = _left;
			expr->right = _right;
			expr->op = op;
			_left = expr;
		}

		return _left;
	}
	Expression* ParseMulDiv(std::string &str)
	{
		Expression* _left = ParseAtom(str);
		while (true)
		{
			Operation op = Operation::NOP;
			std::string remainingStr = str;
			if (!ParseOperator(remainingStr, op) || (op != Operation::MUL && op != Operation::DIV))
				return _left;
			switch (op)
			{
			case Operation::MUL:
			case Operation::DIV:
				break;
			default:
				return _left;
			}
			str = remainingStr;
			Expression* _right = ParseMulDiv(str);
			Expression* expr= new Expression;
			expr->left = _left;
			expr->right = _right;
			expr->op = op;
			_left = expr;
		}

		return _left;
	}
	Expression* ParseAtom(std::string &str)
	{
		Expression* expr = new Expression;
		if (!ParseNum(str, expr->value))
			throw std::runtime_error("");
		return expr;
	}
	bool ParseNum(std::string &expression, std::int64_t &result)
	{
		std::string remainingStr = expression;
		size_t numSize = 0;
		bool isnegative = false;
		if (remainingStr[0] == '-')
		{
			remainingStr = remainingStr.substr(1);
			SkipSpaces(remainingStr);
			isnegative = true;
		}
		SkipSpaces(remainingStr);
		if (remainingStr.size() > 0 && isdigit(remainingStr[0]))
		{
			while (numSize < remainingStr.size() && isdigit(remainingStr[numSize]))
				++numSize;
			result = atoi(remainingStr.substr(0, numSize).c_str());
			if (isnegative) {
				result *= -1;
				op = Operation::ADD;
			}
			expression = remainingStr.substr(numSize);
			return true;
		}
		return false;
	}
	bool ParseOperator(std::string &expression, Operation &op)
	{
		std::string remainingStr = expression;
		if (remainingStr.empty())
		{
			op = Operation::NOP;
			return false;
		}
		switch (remainingStr[0])
		{
		case '+':
			op = Operation::ADD; break;
		case '-':
			op = Operation::SUB; break;
		case '*':
			op = Operation::MUL; break;
		case '/':
			op = Operation::DIV; break;
		default:
			op = Operation::NOP; break;
		}
		const bool succeed = (op != Operation::NOP);
		if (succeed)
			expression = remainingStr.substr(1);
		return succeed;
	}
public:
	Expression(){}
	Expression(const std::string& str)
	{
		std::string exp = deletedoublemines(SkipSpaces(str));
		Expression* rexp=ParseAddSub(exp);
		if (!exp.empty()) {
			throw std::runtime_error("");
		}
		result = Calculate(rexp);
	}
	std::int64_t show_result() {
		return result;
	}
};

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cout << "error";
		return 1;
	}
	std::string str= argv[1];
	try
	{
		Expression r(str);
		std::cout << r.show_result() << std::endl;
	}
	catch (const std::runtime_error)
	{
		std::cout << "error";
		return 1;
	}
	return 0;
}
