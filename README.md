# ESP01nRelayBoards
There some boards on the market that have 1 or more relays and a connector for an ESP01. This project provides a small test programm for those boards
## 1 Relay Board
Relay is directly connection to GPIO0
## 2-8 Relay Boards
There is an STM8 processor on the board that takes Messages over the serial port.

Messages are build as follows
A0     0i                   0s                           Ac
Fix    Relay Index 01 - 08  Status 01 = High 00 = Low    Checksumm A (Relay Index + Status)

Example Relay 1 to high
A0 01 01 A2

Example Relay 8 to low 
A0 08 00 A8
