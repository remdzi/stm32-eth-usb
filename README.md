# Install toolchain
```
sudo apt install cmake gcc-arm-none-eabi openocd gdb-multiarch
```

# Build
```
mkdir build
cd build
cmake ..
make
```

# Flash
```
make flash
```

# Debug
```
make gdb
```
