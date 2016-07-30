# rtl_fftmax
A small utility to find out if there is some signal above noise in a rtl_sdr IQ file
<br/>
rtl_fftmax [-f frequency] [-s sample rate] [-n #bins] < file.iq > 2-column-text-file
<br/>
Reads stdin, writes stdout.
<br/>
The input file is a raw rtl_sdr IQ file; the output is a 2-column table (frequency, value) to be used with gnuplot (or a spreadsheet)
<br/>
See also https://github.com/xofc/rtl_dump
