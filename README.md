# Cpp_EventHandler
以回调事件为底层的纯C++事件委托工具


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
