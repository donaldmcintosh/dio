# dio
dio - device I/O analysis tool

![dio png](https://github.com/donaldmcintosh/dio/blob/master/site/dio.png)

dio is a device I/O analysis tool.  It takes a device name and a time interval in seconds as parameters.  dio sam-
ples kernel statistics at the specified time interval and uses them to derive useful real-time and cumulative  per-
formance statistics.  It presents the output in a standard terminal window using the standard curses library, which
contains the output on visible terminal.  The approach offers a more intuitive visual presentation of the data.

The device name can be an entire disk, a disk slice, a metadevice or any other kind of I/O device.  The  -l  option
can be used to list the names of all available I/O devices.

The abbreviated column headings on the output are explained below:

Col   | Description
----- | ---------------------------------------------------------
br    |  number of bytes read during time interval
bw    |  number of bytes written during time interval
rkb/s |  average   kilobytes   per  second  read  from device
wkb/s |  average kilobytes per second written  to  the device
%r:%w |  ratio  of  data read to data written.  Useful for visualising  the  changing  direction  of data flow
ro    |  number of read operations
wo    |  number of write operations
ios   |  I/O operations current in progress
rt    |  time spent reading
wt    |  time spent writing

Also  included  in  the output is maximum recorded read rate recorded, maximum write rate, total megabytes read and
total megabytes written during the period that dio was running.

Both rt and wt fields can legitimately be greater than the elapsed time of the interval.  These present the sum  of
time  spent actioning read and write requests and these will often be done in parallel when the I/O device is busy.
In such cases the total write/read time can be many time greater than elapsed time, depending on the capability  of
the device.

