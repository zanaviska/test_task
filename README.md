# test task repo

to build:
```
mkdir build
cd build
cmake ..
cmake --build .
```
 
this few lines should create folder **bin**. There should be 3 executables inside folder:
1) ```bussines_logic.out``` for calculating result price. communicate with **server** executable via shared memory
2) ```server.out``` for connecting bussines logic executable with client. Connection with client via tcp connection, with bussines logic via shared memory
3) ```client.out``` for interfacing with user. Comunicate with server via tcp connection

**server** and **bussines_logic** is stable to restarting another processes. **client** may fail when other service is down
