# esp3266_rtos_playground

## Make workflow
make app PYTHON=python3

make flash

make monitor

make flash monitor

make -j4 app-flash monitor

## CMake workflow
mkdir build

cmake -Bbuild/ -H. -DPYTHON=python3

make -C build -j4 app
