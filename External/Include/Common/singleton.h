#pragma once


using EXIT = void(*)(void);

template <typename T>
class singleton
{
    static T* m_Inst;

public:
    static T* GetInst()
    {
        if (nullptr == m_Inst)
        {
            m_Inst = new T;
            atexit(&singleton<T>::Destroy);
        }

        return m_Inst;
    }

    static void Destroy()
    {
        if (nullptr != m_Inst)
        {
            delete m_Inst;
            m_Inst = nullptr;
        }
    }
};

template <typename T>
T* singleton<T>::m_Inst = nullptr;


#define SINGLE(TYPE) private:\
						TYPE();\
						TYPE(const TYPE& _Other) = delete;\
						~TYPE();\
						TYPE& operator =(const TYPE& _Other) = delete;\
						friend class singleton<TYPE>;
