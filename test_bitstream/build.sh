yosys test.ys
nextpnr-ice40 --lp384 --package qn32 --json test.json --pcf test.pcf --asc test.asc
icepack test.asc test.bin
