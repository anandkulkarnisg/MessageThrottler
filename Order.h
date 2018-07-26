#include<iostream>
#include<string>

#ifndef Order_H
#define Order_H

class Order
{
	private:
		long m_orderSequenceId;			// Sequence id is assigned interally to track order genreation , it can be replaced with timestamp or some sort of sequence generator logic.
		long m_orderId;					// The orderId is a unique long number starting with some seed and grows sequentially for the day.
		char m_orderSide;				// The orderSide is either Buy [ B] / Sell [ S ].
		char m_orderType;				// The orderType can be either New/Add [ A ] , Modify [ M ] or Cancel/Remove [ X ].
		double m_orderPrice;			// The price of underlying symbol , this is usually double/float type.
		long m_orderSize;				// The size is always a positive integer number and cant be either zero/-ve.
		std::string m_orderSymbolName;	// The symbol is usually of fixed size like ex:- 12 characters. For simplicity std::string is used.
	public:

		// These are state attributes of the orderActionType which are Addition('A') / Modification('M') / Removal('X') of the order.
		// Addition is the initial arrival of the Order for first time.
		// Modification and removal are the other two states of the order.

		static const char orderNew;
		static const char orderModify;
		static const char orderCancel;

		static const char orderTypeBuy;
		static const char orderTypeSell;

        static const int orderSymbolLength;

		// Required constructors.
		Order();
		Order(const long&, const long&, const char&, const char&, const double&, const long&, const std::string&);
		Order(const Order&);
		Order& operator=(const Order&);
	
		// Move constructors.
		Order(Order&&);
		Order& operator=(Order&&);
	
		// some important Getters are included here.
		long getOrderSequenceId() const;
		long getOrderId() const;
		char getOrderSide() const;
		char getOrderType() const;
		double getOrderPrice() const;
		long getOrderSize() const;
		std::string getOrderSymbolName() const;

		// We need a comparison operator to define the sorting criteria for the Orders. 
        friend bool operator < (const Order& lhs, const Order& rhs);

		// We need some helper functions to implement display of all Order properties.
		std::string getOrderMessage() const;	
};

#endif
