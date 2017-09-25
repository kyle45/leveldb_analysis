template<classT>
class Autoptr //采用资源的转移方法管理内存，在它的拷贝构造和赋值运算符引起会出现问题
{
public:
	Autoptr(T *p=NULL )
		:ptr(p)
	{}
	Autoptr(Autoptr<T>&ap)
		:ptr(ap.ptr)
	{
		ap.ptr = NULL;//新对象构造成功之后，原来对象指针为NULL；保证free的时候释放1次
				
	}
 
	Autoptr<T>& operator=(Autoptr<T>&ap)
	{
		if(this != &ap)
		{
			if(ap.ptr)
			{
				delete ptr;
			}
			ptr = ap.ptr;
			ap.ptr = NULL;
		}
		return *this;
	}
 
	T* operator*()
	{
		return *ptr;
	}
	T* operator->()
	{
		return ptr;
	}
	void get_ptr()
	{
		return ptr;
	}
	~Autoptr()
	{
		if(ptr)
		{
			delete ptr;
			ptr = NULL;
		}
	}
private:
	T* ptr;
};