#include "OrderValidator.h"

long OrderValidator::orderSequenceId = 0;
int OrderValidator::orderLayoutTokens = 6;

std::tuple<std::pair<bool,std::string>,Order> OrderValidator::validateOrder(const std::string& inputOrderString)
{
	// Let us have a blank orderType that is returned and not used if the message parsing fails.
	Order invalidOrder(0,0,'X','Y',0.0,-1,"BADORDER");
	std::string failMessage = "";

	// First let us try to tokenize the inputOrderString and validate if all parameters are correct and valid. At any stage if validation fails then the failed order is moved in Main application
	// layer to a failed queue or bad message queue.

	// First of all let us try to tokenize the message and identify all the important parts of it.
	std::vector<std::string> orderItems;
	char_separator<char> seperator(",");
	tokenizer<char_separator<char>> tokens(inputOrderString, seperator);
	for(const auto& iter : tokens)
		orderItems.push_back(iter);

	// If there are 6 elements then we are fine , else reject as bad message with different number of fields.
	if(orderItems.size() != OrderValidator::orderLayoutTokens)
	{
		failMessage = "Invalid Message. Wrong number of tokens in the message. ";
		failMessage += std::to_string(static_cast<long>(orderItems.size()));
		failMessage += " items were found.";
		return(std::make_tuple(std::make_pair(false,failMessage), invalidOrder));
	}

	// Check the order type i.e action length should be 1.
	std::string orderType = orderItems[0];
	if(orderType.length() != 1)
	{
		failMessage = "Invalid order action type length : " + orderType;
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	// Order type should be either A/X/M.
	if(std::toupper(orderType[0]) != Order::orderAddition && std::toupper(orderType[0]) != Order::orderRemove && std::toupper(orderType[0]) != Order::orderModify)
	{
		failMessage = "Invalid order action type : " + orderType + ". It should be either A(Add)/X(Remove)/M(Modify)";
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	// Now extract the orderId. It should be an integer / long assumed here.
	long orderId = 0;
	try
	{
		orderId = std::stol(orderItems[1]);
	} catch(const std::invalid_argument& exception)
	{
		failMessage = "Failed to convert the orderId into proper long : Exception is : ";
		std::string msgType(exception.what());
		failMessage += msgType;
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	std::string orderSide = orderItems[2];
	if(orderSide.length() != 1)
	{
		failMessage = "Invalid OrderSide Length : " + orderSide + ". It should be either B/S";
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	if(orderSide[0] != Order::orderTypeBuy && orderSide[0] != Order::orderTypeSell)
	{
		failMessage = "Invalid orderSide : " + orderSide + ". It should be either B/S";
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	long orderQty = 0;
	try
	{
		orderQty = std::stol(orderItems[3]);
	} catch(const std::invalid_argument& exception)
	{
		failMessage = "Failed to convert the " + orderItems[3] + " into valid order quantity long type.Exception is : " + exception.what();
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	if(orderQty <= 0)
	{
		failMessage = "The order quantity has to be a +ve long. Currently it is : " + orderItems[3];
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	double orderPrice = 0;
	try
	{
		orderPrice = std::stod(orderItems[4]);
	} catch(const std::invalid_argument& exception)
	{
		failMessage = "Failed to convert the " + orderItems[4] + " into valid order price od double type.Exception is : " + exception.what();
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	if(orderPrice <= 0)
	{
		failMessage = "The order price has to be a +ve price. Currently it is : " + orderItems[4];
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	std::string symbolName = orderItems[5];
	if(symbolName.length() != Order::orderSymbolLength)
	{
		failMessage = "The order does not have a proper symbol Length of size 12. Please check";
		return(std::make_tuple(std::make_pair(false,failMessage),invalidOrder));
	}

	// This means we have a valid order if we reached here.Construct and return here.   
	Order validOrder(orderSequenceId, orderId, (orderSide.c_str())[0], (orderType.c_str())[0],orderPrice, orderQty, symbolName);
	orderSequenceId++;
	return(std::make_tuple(std::make_pair(true,""),validOrder));
}
