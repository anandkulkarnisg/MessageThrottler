#include "Order.h"

const char Order::orderAddition = 'A';
const char Order::orderModify   = 'M';
const char Order::orderRemove   = 'X';

const char Order::orderTypeBuy = 'B';
const char Order::orderTypeSell = 'S';

const int Order::orderSymbolLength = 12;

Order::Order()
{
	
}

// Default Constructor parameterized.
Order::Order(const long& orderSequenceId, const long& orderId, const char& orderSide, const char& orderType, const double& orderPrice, const long& orderSize, const std::string& orderSymbolName) : m_orderSequenceId(orderSequenceId), m_orderId(orderId), m_orderSide(orderSide), m_orderType(orderType), m_orderPrice(orderPrice), m_orderSize(orderSize), m_orderSymbolName(orderSymbolName) 
{

}

// Copy constructor.
Order::Order(const Order& copyRef)
{
	this->m_orderSequenceId = copyRef.m_orderSequenceId;
	this->m_orderId = copyRef.m_orderId;
	this->m_orderSide = copyRef.m_orderSide;
	this->m_orderSize = copyRef.m_orderSize;
	this->m_orderType = copyRef.m_orderType;
	this->m_orderPrice = copyRef.m_orderPrice;
	this->m_orderSymbolName = copyRef.m_orderSymbolName;
}

// Assignment operator constructor.
Order& Order::operator=(const Order& assignRef)
{
	this->m_orderSequenceId = assignRef.m_orderSequenceId;
	this->m_orderId = assignRef.m_orderId;
	this->m_orderSide = assignRef.m_orderSide;
	this->m_orderSize = assignRef.m_orderSize;
	this->m_orderType = assignRef.m_orderType;
	this->m_orderPrice = assignRef.m_orderPrice;
	this->m_orderSymbolName = assignRef.m_orderSymbolName;
	return(*this);
}

Order::Order(Order&& moveRef)
{
	this->m_orderSequenceId = moveRef.m_orderSequenceId;
    this->m_orderId = moveRef.m_orderId;
    this->m_orderSide = moveRef.m_orderSide;
    this->m_orderSize = moveRef.m_orderSize;
    this->m_orderType = moveRef.m_orderType;
    this->m_orderPrice = moveRef.m_orderPrice;
    this->m_orderSymbolName = std::move(moveRef.m_orderSymbolName);
}

Order& Order::operator=(Order&& moveRef)
{
    this->m_orderSequenceId = moveRef.m_orderSequenceId;
    this->m_orderId = moveRef.m_orderId;
    this->m_orderSide = moveRef.m_orderSide;
    this->m_orderSize = moveRef.m_orderSize;
    this->m_orderType = moveRef.m_orderType;
    this->m_orderPrice = moveRef.m_orderPrice;
    this->m_orderSymbolName = std::move(moveRef.m_orderSymbolName);
    return(*this);
}

// All getters are below.
long Order::getOrderSequenceId() const { return(m_orderSequenceId); }
long Order::getOrderId() const { return(m_orderId); }
char Order::getOrderSide() const { return(m_orderSide); }
char Order::getOrderType() const { return(m_orderType); }
double Order::getOrderPrice() const { return(m_orderPrice); }
long Order::getOrderSize() const { return(m_orderSize); }
std::string Order::getOrderSymbolName() const { return(m_orderSymbolName); }

// Implementation of sorting criteria for the Order.

bool operator < (const Order& lhs, const Order& rhs)
{
	// First priority is given to cancel orders to move ahead. 
	if(lhs.getOrderType() == Order::orderRemove && rhs.getOrderType() != Order::orderRemove)
		return(true);

	if(rhs.getOrderType() == Order::orderRemove && lhs.getOrderType() != Order::orderRemove)
		return(false);

	// If both are cancels then seqId / arrival timestamp gets priority.
	if(lhs.getOrderType() == Order::orderRemove && rhs.getOrderType() == Order::orderRemove)
	{
		if(lhs.getOrderSequenceId() < rhs.getOrderSequenceId())
			return(true);
		else
			return(false);	
	}
	
	// If both are not cancel then queue as per seqId.
	if(lhs.getOrderType() != Order::orderRemove && rhs.getOrderType() != Order::orderRemove)
	{
		if(lhs.getOrderSequenceId() < rhs.getOrderSequenceId())
			return(true);
		else
			return(false);
	}
}

std::string Order::getOrderMessage() const
{

	// Order Layout.
	// seqId, orderType, orderId, orderSide, orderQty, orderPrice, orderSymbol
	// A,100001,B,1000,101.50,SINTEXSYMBOL

	std::string returnMessage = "orderSequenceId = ";
	returnMessage += std::to_string(m_orderSequenceId);

	returnMessage += ", orderType = ";
	returnMessage += m_orderType;
	returnMessage += ", orderId = ";
	returnMessage += std::to_string(m_orderId);
	returnMessage += ", orderSide = ";
	std::string str(1,m_orderSide);
	returnMessage += str;
	returnMessage += ", orderQty = ";
	returnMessage += std::to_string(m_orderSize);
	returnMessage += ", orderPrice = ";
	returnMessage += std::to_string(m_orderPrice);
	returnMessage += ", orderSymbol = ";
	returnMessage += m_orderSymbolName;
	return(returnMessage);
}


