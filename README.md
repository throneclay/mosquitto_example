# mosquitto c++ client example

mosquitto client example using protobuf

first launch scripts/get\_mosq\_lib.sh to get the source of mosquitto, it will compile the lib of mosquitto.
```
cd scripts
./get_mosq_lib.sh
```

then build the project.

```
mkdir build
cd build
cmake ..
make 
```

If the protobuf not found. vim CMakeLists.txt and modify the protobuf root path
