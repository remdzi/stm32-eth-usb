#!/bin/sh

openocd -f ../st_nucleo_f2.cfg -c init -c "reset halt" -c "flash write_image erase $1" -c "reset halt" &
OPENOCD_PID=$!
echo "OPENOCD_PID = $OPENOCD_PID"

sleep 2
gdb-multiarch -ex 'target remote localhost:3333' $1 || true

kill $OPENOCD_PID
