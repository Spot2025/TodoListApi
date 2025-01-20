# Todo List API

### Description
This API does what it says on this link: https://roadmap.sh/projects/todo-list-api

### Build
This C++ project is based on Drogon framework, so you should install it first.

Here's install guide: https://drogonframework.github.io/drogon-docs/#/ENG/ENG-02-Installation

Then you should run database by docker (you need docker for run):
```
docker-compose up --build
```

Then you should build project itself and run:
```
mkdir build && cd build
cmake ..
make TodoListApi
./TodoListApi
```

After running project you can run tests:
```
cd build
make TodoListApi_test
cd test
./TodoListApi_test
```