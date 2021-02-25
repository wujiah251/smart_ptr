#include <functional>

// shared_ptr

template <typename T>
class shared_ptr
{
public:
    // 默认构造函数
    shared_ptr();
    // 参数构造函数，禁止隐式类型转换
    explicit shared_ptr(T *p);
    // 拷贝构造函数
    shared_ptr(const shared_ptr &);
    //
    shared_ptr(T *, str::function<void(T *)>);
    // 赋值函数
    shared_ptr<T> &operator=(const shared_ptr &other);
    // 解除引用
    T &operator*() const;
    // 箭头运算符
    T *operator->() const;
    // 析构函数
    ~shared_ptr();

    // 向bool类型的转换
    explicit operator bool() const;

    // 判断是否是独占的
    bool unique();
    // 重置当前智能指针
    void reset();
    void reset(T *p);
    void reset(T *, std::function<void(T *)> del);
    T *release();
    T *get() const;

private:
    static std::function<void(T *)> default_del;

private:
    T *m_project = nullptr;
    unsigned *m_use_count = nullptr;
    std::function<void(T *)> m_del = default_del;
};

// 默认的deleter方法
template <typename T>
std::function<void(T *)> shared_ptr<T>::default_del = [](T *p) {delete p; p=nullptr; };

// make_shared方法
template <typename T, typename... Args>
shared_ptr<T> make_shared(Args &&...Args)
{
    shared_ptr<T> sp(new T(std::forward<Args>(args)...));
    return sp;
}

// 默认构造函数
template <typename T>
shared_ptr<T>::shared_ptr()
    : m_project(nullptr), m_use_count(new unsigned(1))
{
}

// 参数构造函数
template <typename T>
shared_ptr<T>::shared_ptr(T *p)
    : m_project(p), m_use_count(new unsigned(1))
{
}

// 拷贝构造函数
template <typename T>
shared_ptr<T>::shared_ptr(const shared_ptr &other)
    : m_project(other.m_project),
      m_use_count(other.m_use_count),
      m_del(other.m_del)
{
}

// 重载复制函数
template <typename T>
shared_ptr<T> &shared_ptr<T>::operator=(const shared_ptr &other)
{
    if (&other == *this)
        return *this;

    //共享deleter
    m_del = other.m_del;
    // 增加右侧引用计数
    ++(*other.m_use_count);
    // 递减本对象的引用计数
    if (--(*m_use_count) == 0)
    {
        // 如果引用计数为0，释放分配对象
        m_del(m_project);
        delete m_use_count;
    }
    // 复制对象
    m_use_count = other.m_use_count;
    m_project = other.m_project;
    return *this;
}

// 解引用
template <typename T>
T &shared_ptr<T>::operator*() const
{
    return *m_project;
}

template <typename T>
T *shared_ptr<T>::operator->() const
{
    return &this->operator*();
}

// 析构函数
template <typename T>
shared_ptr<T>::~shared_ptr()
{
    if (--(*m_use_count) == 0)
    {
        m_del(m_project);
        m_project = nullptr;
        delete m_use_count;
        m_use_count = nullptr;
    }
}

// bool类型转换
template <typename T>
shared_ptr<T>::operator bool() const
{
    return m_project != nullptr;
}

template <typename T>
bool shared_ptr<T>::unique()
{
    return *m_use_count == 1;
}

template <typename T>
void shared_ptr<T>::reset()
{
    (*m_use_count)--;
    if (m_use_count == 0)
    {
        m_del(m_project);
    }
    m_project = nullptr;
    *m_use_count = 1;
    m_del = default_del;
}

template <typename T>
void shared_ptr<T>::reset(T *)
{
    (*m_use_count)--;
    if (*m_use_count == 0)
    {
        m_del(m_project);
    }
    m_project = p;
    *m_use_count = 1;
    m_del = default_del;
}

template <typename T>
void shared_ptr<T>::reset(T *p, std::function<void(T *)> del)
{
    reset(p);
    m_del = del;
}

template <typename T>
T *shared_ptr<T>::release()
{
    if (--(*m_use_count) == 0)
    {
        *m_use_count = 1;
    }
    auto p = m_project;
    m_project = nullptr;
    return p;
}

template <typename T>
T *shared_ptr<T>::get() const
{
    return m_project;
}