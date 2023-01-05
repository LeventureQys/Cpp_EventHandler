/*!
* \file IDelegate.h
*
* \author Administrator
* \date 七月 2020
*
* 委托，由于暂找不到std::function对象间目标比较方法，特开委托处理
*/
#pragma once
#include <list>
#include <typeinfo>

/////////委托接口声明//////////////////////////////////////////////////
template<typename ReturnType, typename... ParamType>
class IDelegate
{
public:
	virtual ~IDelegate() {};

	virtual bool IsType(const std::type_info& _type) = 0;
	virtual ReturnType invoke(ParamType... params) = 0;
	virtual bool equal(IDelegate<ReturnType, ParamType...> *_delegate) const = 0;
};
/////////委托接口声明//////////////////////////////////////////////////

///////一般方法、类静态方法委托///////////////////////////
template<typename ReturnType, typename... ParamType>
class CStaticDelegate : public IDelegate<ReturnType, ParamType...>
{
public:
	typedef ReturnType(*Func)(ParamType...);

	CStaticDelegate(Func func) : m_func(func) {};
	virtual ~CStaticDelegate() {};

	virtual bool IsType(const std::type_info& _type)
	{
		return typeid(CStaticDelegate<ReturnType, ParamType...>) == _type;
	}

	virtual ReturnType invoke(ParamType... params)
	{
		return m_func(params...);
	}

	virtual bool equal(IDelegate<ReturnType, ParamType...> *_delegate) const
	{
		if (!_delegate || !_delegate->IsType(typeid(CStaticDelegate<ReturnType, ParamType...>))) return false;
		CStaticDelegate<ReturnType, ParamType...>* cast = static_cast<CStaticDelegate<ReturnType, ParamType...>*>(_delegate);
		return cast->m_func == m_func;
	}

	Func m_func;
};

// 委托特化
template<typename ReturnType, typename... ParamType>
class CStaticDelegate<ReturnType(*)(ParamType ...)> : public IDelegate<ReturnType, ParamType...>
{
public:
	typedef ReturnType(*Func)(ParamType...);

	CStaticDelegate(Func func) : m_func(func) {};
	virtual ~CStaticDelegate() {};

	virtual bool IsType(const std::type_info& _type)
	{
		return typeid(CStaticDelegate<ReturnType, ParamType...>) == _type;
	}

	virtual ReturnType invoke(ParamType... params)
	{
		return m_func(params...);
	}

	virtual bool equal(IDelegate<ReturnType, ParamType...> *_delegate) const
	{
		if (!_delegate || !_delegate->IsType(typeid(CStaticDelegate<ReturnType, ParamType...>))) return false;
		CStaticDelegate<ReturnType, ParamType...>* cast = static_cast<CStaticDelegate<ReturnType, ParamType...>*>(_delegate);
		return cast->m_func == m_func;
	}

	Func m_func;
};
///////一般方法、类静态方法委托///////////////////////////


////////类普通成员方法委托////////////////////////////////////////////
template<typename classType, typename ReturnType, typename... ParamType>
class CMethodDelegate : public IDelegate<ReturnType, ParamType...>
{
public:
	typedef ReturnType(classType::*Func)(ParamType...);

	CMethodDelegate(Func func, classType* pObject) : m_pObject(pObject), m_func(func) {};
	virtual ~CMethodDelegate() {};

	virtual bool IsType(const std::type_info& _type)
	{
		return typeid(CMethodDelegate<classType, ReturnType, ParamType...>) == _type;
	}

	virtual ReturnType invoke(ParamType... params)
	{
		return (m_pObject->*m_func)(params...);
	}

	virtual bool equal(IDelegate<ReturnType, ParamType...> *_delegate) const
	{
		if (!_delegate || _delegate->IsType(typeid(CMethodDelegate<classType, ReturnType, ParamType...>))) return false;
		CMethodDelegate<classType, ReturnType, ParamType...>* cast = static_cast<CMethodDelegate<classType, ReturnType, ParamType...>*>(_delegate);
		return (cast->m_pObject == m_pObject && cast->m_func == m_func);
	}

	classType*	m_pObject;
	Func		m_func;
};

// 委托特化
template<typename classType, typename ReturnType, typename... ParamType>
class CMethodDelegate<classType, ReturnType(classType::*)(ParamType...)> : public IDelegate<ReturnType, ParamType...>
{
public:
	typedef ReturnType(classType::*Func)(ParamType...);

	CMethodDelegate(Func func, classType* pObject) : m_pObject(pObject), m_func(func) {};
	virtual ~CMethodDelegate() {};

	virtual bool IsType(const std::type_info& _type)
	{
		return typeid(CMethodDelegate<classType, ReturnType, ParamType...>) == _type;
	}

	virtual ReturnType invoke(ParamType... params)
	{
		return (m_pObject->*m_func)(params...);
	}

	virtual bool equal(IDelegate<ReturnType, ParamType...> *_delegate) const
	{
		if (!_delegate || _delegate->IsType(typeid(CMethodDelegate<classType, ReturnType, ParamType...>))) return false;
		CMethodDelegate<classType, ReturnType, ParamType...>* cast = static_cast<CMethodDelegate<classType, ReturnType, ParamType...>*>(_delegate);
		return (cast->m_pObject == m_pObject && cast->m_func == m_func);
	}

	classType*	m_pObject;
	Func		m_func;
};
////////类普通成员方法委托////////////////////////////////////////////


/////////全局函数/////////////////////////////////////////////////
template< typename T>
CStaticDelegate<T>* newDelegate(T func)
{
	return new CStaticDelegate<T>(func);
}
template< typename T, typename F>
CMethodDelegate<T, F>* newDelegate(T * _object, F func)
{
	return new CMethodDelegate<T, F>(func, _object);
}
/////////全局函数/////////////////////////////////////////////////


///////多播委托/////////////////////////////////////////////////
template<typename ReturnType, typename... ParamType>
class CMultiDelegate
{
public:
	typedef std::list<IDelegate<ReturnType, ParamType...>*> ListDelegate;
	typedef typename ListDelegate::iterator ListDelegateIterator;
	typedef typename ListDelegate::const_iterator ConstListDelegateIterator;

	CMultiDelegate() {};
	CMultiDelegate(const CMultiDelegate& rhs) = delete;				// 禁用拷贝构造
	CMultiDelegate& operator=(const CMultiDelegate& rhs) = delete;	// 禁用赋值
	~CMultiDelegate() { Clear(); };

	void Clear()
	{
		for (ListDelegateIterator it = m_ListDelegates.begin(); it != m_ListDelegates.end();)
		{
			if (*it)
			{
				delete *it;
				*it = nullptr;

				it = m_ListDelegates.erase(it);
			}
			else
				++it;
		}
	}

	CMultiDelegate<ReturnType, ParamType...>& operator+=(IDelegate<ReturnType, ParamType...>* _delegate)
	{
		for (auto& delgitem : m_ListDelegates)
		{
			if (delgitem && delgitem->equal(_delegate))
			{
				delete _delegate;

				return *this;
			}
		}

		m_ListDelegates.push_back(_delegate);
		return *this;
	}

	CMultiDelegate<ReturnType, ParamType...>& operator-=(IDelegate<ReturnType, ParamType...>* _delegate)
	{
		for (auto& delgitem : m_ListDelegates)
		{
			if (delgitem && delgitem->equal(_delegate))
			{
				// 地址不同时才删除，避免函数末尾处理进行删除发生删除重复
				if (delgitem != _delegate)
				{
					delete delgitem;
					delgitem = nullptr;
				}

				m_ListDelegates.erase(delgitem);
				break;
			}
		}

		delete _delegate;		// 删除参数
		return *this;
	}

	void operator()(ParamType... params)
	{
		for (auto& delgitem : m_ListDelegates)
		{
			if (delgitem)
			{
				delgitem->invoke(params...);
			}
		}
	}

private:
	ListDelegate m_ListDelegates;
};
///////多播事件/////////////////////////////////////////////////






//////////////////使用示例///////////////////////////////////
/*
eg1:

CMultiDelegate<void, int> e;		// 返回值int，参数类型int

// 添加委托
e += newDelegate(NormalFunc);
e += newDelegate(A::StaticFunc);
e += newDelegate(&A(), &A::MemberFunc);

// 调用
e(1);

*/
/*
eg2:

// 受限创建一个返回值为int，无参数需求的一个委托
CMultiDelegate<void> e;

//将三个函数注册到该委托中
e += newDelegate(NormalFunc);
e += newDelegate(A::StaticFunc);
e += newDelegate(&A(), &A::MemberFunc);

//调用
e();
*/