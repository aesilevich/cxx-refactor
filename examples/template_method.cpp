
template <typename T1, typename T2>
class my_class {
public:
    void foo(T1 x, T2 y);

private:
    T1 x_;
    T2 y_;
};


template <typename X1, typename X2>
void my_class<X1, X2>::foo(X1 x, X2 y) {    
    my_class<X1, X2> a;
    X2 tmp = y;
}


void func(my_class<int, float> x) {
}
