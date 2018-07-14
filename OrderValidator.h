#include<iostream>
#include<string>
#include<tuple>
#include<boost/tokenizer.hpp>
#include "Order.h"

using namespace std;
using namespace boost;

// This is a validator/helper class that implements the validation of building an order object from string by parsing / validating etc.
// If successfull it returns true of else false. 

// The status of return is true then the string message will be generally empty and Order object returned can be used further.
// If the status of return is false then the String message indicates validation failure nature and the returned Order will be an invalid one which should not be used further.

#ifndef OrderValidator_H
#define OrderValidator_H

class OrderValidator
{
private:
	static long orderSequenceId;

public:
	std::tuple<std::pair<bool,std::string>,Order> validateOrder(const std::string&);
};

#endif

