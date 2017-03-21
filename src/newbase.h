#ifndef BASE_H
#define BASE_H

//declares and defines main base classes for composite structure

#include <string>
#include <cstdlib>
using namespace std;


// -----BASE CLASS-----
class Base {
    public:
    //executes the leaf or composite node in the tree
    virtual bool execute(int in, int out) = 0;
    virtual string getString() = 0;
};
// -----END BASE CLASS -----


// -----CONNECTOR CLASS-----
class Connector: public Base {
    protected:
		Base* lhs;
		Base* rhs;
    public:
    //constructors
    Connector(Base* left, Base* right)
		:	lhs(left), rhs(right) {}
    Connector()
		:	lhs(NULL), rhs(NULL) {}

    virtual bool execute(int in, int out) = 0; //execute function to be overridden by derived classes
    virtual string getString(){
		return "error";
	}
};
// -----END CONNECTOR CLASS-----

#endif
