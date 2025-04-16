#pragma once

template <typename T>
class Ptr
{
	T* m_Ptr;

public:
	T* Get() const { return m_Ptr; }
	T* operator->() const { return m_Ptr; }
	T** GetAdressOf() const { return &m_Ptr; }

	bool operator ==(T* _Ptr) const { return m_Ptr == _Ptr; }
	bool operator !=(T* _Ptr) const { return m_Ptr != _Ptr; }
	bool operator ==(const Ptr<T>& _Ptr) const { return m_Ptr == _Ptr.m_Ptr; }
	bool operator !=(const Ptr<T>& _Ptr) const { return m_Ptr != _Ptr.m_Ptr; }

	Ptr<T>& operator =(T* _Ptr)
	{
		if (m_Ptr == _Ptr)
			return *this;

		if (nullptr != m_Ptr)
			m_Ptr->Release();

		m_Ptr = _Ptr;

		if (nullptr != m_Ptr)
			m_Ptr->AddRef();

		return *this;
	}

	Ptr<T>& operator =(const Ptr<T>& _Ptr)
	{
		if (m_Ptr == _Ptr.m_Ptr)
			return *this;

		if (nullptr != m_Ptr)
			m_Ptr->Release();

		m_Ptr = _Ptr.m_Ptr;

		if (nullptr != _Ptr.m_Ptr)
			m_Ptr->AddRef();

		return *this;
	}

	Ptr()
		: m_Ptr(nullptr)
	{
	}

	Ptr(T* _Ptr)
		: m_Ptr(_Ptr)
	{
		if (nullptr != m_Ptr)
			m_Ptr->AddRef();
	}

	Ptr(const Ptr<T>& _Ptr)
		: m_Ptr(_Ptr.m_Ptr)
	{
		if (nullptr != m_Ptr)
			m_Ptr->AddRef();
	}

	~Ptr()
	{
		if (nullptr != m_Ptr)
			m_Ptr->Release();
	}
};

template <typename T>
bool operator ==(void* _asset, const Ptr<T>& _Ptr)
{
	return _Ptr == static_cast<T*>(_asset);
}

template <typename T>
bool operator !=(void* _asset, const Ptr<T>& _Ptr)
{
	return _Ptr != static_cast<T*>(_asset);
}
