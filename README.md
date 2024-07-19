# cxx-refactor - C++ Refactoring Tool

## How to build

```bash
git clone git@github.com:aesilevich/cxx-refactor.git
mkdir build_dir
cd build_dir
cmake ../cxx-refactor -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Example usage
Execute the following command from build directory:
```bash
cd build_dir
./bin/cxx-refactor template-parameter-remove --input=../cxx-refactor/examples/template_method.cpp --position=template_method.cpp:2:33
```

The command will print modified source with removed template parameter:
```cpp

template <typename T1>
class my_class {
public:
    void foo(T1 x, ?? y);

private:
    T1 x_;
    ?? y_;
};


template <typename X1>
void my_class<X1>::foo(X1 x, ?? y) {    
    my_class<X1> a;
    ?? tmp = y;
}


void func(my_class<int> x) {
}
```