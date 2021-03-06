#include "../std_lib_facilities.h"
#include "common_types.h"
#include "tokenClass.h"
#include "TokenStream.h"
#include "variables.h"
#include "grammar.h"

extern Token_stream ts;

tValType getExpression(){
	tValType left = getTerm();
	Token t = ts.getToken();
	while (true) {
		switch (t.kind){
		case PLUS:
			left += getTerm();
			t = ts.getToken();
			break;
		case MINUS:
			left -= getTerm();
			t = ts.getToken();
			break;
		default:
			ts.putback(t);
			return left;
			break;
		}
	}
}

tValType getTerm(){
	tValType left = getPrimary();
	Token t = ts.getToken();
	while (true)
	{
		switch (t.kind){
		case MUL:
			left *= getPrimary();

			t = ts.getToken();
			break;
		case DIV:{
			tValType right = getPrimary();
			if (right == 0.0)
				error("Divided by zero!");
			left /= right;

			t = ts.getToken();
			break;
				 }
		case MODULO:{
			tValType right = getPrimary();
			if (right == 0.0)
				error("Divided by zero!");
			int iLeft = narrow_cast<int>(left);
			iLeft %= narrow_cast<int>(right);
			left = iLeft;

			t = ts.getToken();
			break;
					}
		default:
			ts.putback(t);
			return left;

		}
	}
}

tValType getPrimary(){
	Token t = ts.getToken();
	switch (t.kind){
	case OPEN_BRACE:
		{
			tValType d = getExpression();
			t = ts.getToken();
			if (t.kind != CLOSE_BRACE)
				error("Closing brace expected");
			return d;
		}
	case NUMBER:
		return t.value;
		break;
	case MINUS:
		return (- getPrimary());
		break;
	case PLUS:
		return getPrimary();
		break;
	case NAME:
		return get_value(t.name);
		break;
	default:
		error("Primary expected");
		return ERR_VALUE;	// not needed, because error() terminates the program
	}
}

tValType getStatement(){
	Token t = ts.getToken();
	switch (t.kind){
	case LET:
		return getDeclaration();
		break;
	default:
		ts.putback(t);
		return getExpression();
	}
}

tValType getDeclaration(){

	Token t = ts.getToken();
	if (t.kind != NAME) error("Name expected");
	

	Token t2 = ts.getToken();
	if (t2.kind != EQUALS) error("= expected");
	tValType d = getExpression();

	add_variable(t.name,d);
	return d;
}