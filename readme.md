# CMUX / TS 0710
This is a cmux / TS 0710 multiplexer protocol implementation as both client and server which can be used to interact with gsm and wifi modules supporting the cmux protocol as well as writing your own cmux server allowing serial connection sharing by multiplexing a number of connections on a single physical medium such as UARTs.

This project was built with portability, extensibility and readability in mind, most functionality has been unit tested and analyzed for memory issues.

## Dependencies
- lwrb - Tilen MAJERLE


# Integration
This library does not implement any timing and retries that are part of the protocol since it will require OS support.

## Client
An example usage can be found in the test_cmux_client.c:cmux_client_startOpenSendCloseScenario.

- cmux_client_init - initializes a new client instance, register callbacks for start, stop, outgoing data (send to UART) and incoming decoded messages
- cmux_client_start - sends the start command (SABM DLCI 0), the code should wait for cmux_client_on_start callback
- cmux_client_ingest - ingest data into cmux client
- cmux_client_terminal_open - opens terminal 1-63 (DLCI 1-63) and register callbacks for terminal 1-63, the code should wait for cmux_client_on_terminal_open callback
- incoming data for terminal 1-63 should arrive through cmux_client_on_terminal_data callback
- cmux_client_terminal_send - sends data to terminal 1-63
- cmux_client_terminal_close - closes terminal 1-63, the code should wait for cmux_client_on_terminal_close
- cmux_client_stop - sends a stop command (UIH MULTIPLEXER_CLOSE_DOWN)
- cmux_client_free - releases the client instance memory

Additionally, its possible to send custom commands and services with cmux_client_request.

## Server
An example usage can be found in test_cmux_server.c

- cmux_server_init - initializes a new server instance, register callbacks for start, stop, outgoing data (send to UART) and incoming decoded messages
- cmux_server_ingest - ingest data into cmux server
- cmux_server_register_terminal - registers a DLCI 1-63 with terminal data, open and close events
- cmux_server_terminal_send - sends data to terminal 1-63
- cmux_server_terminal_close - closes terminal cmux_server_terminal_close
- cmux_server_free - releases the server instance memory

To implement custom services, incoming decoded messages should be processed and replies cmux_server_request and cmux_server_reply should be used.

# TODO
- Verify Lengths
- Verify Coverage

# Generate Documentation
```
doxygen .doxygen
```

# Acronyms
DLCI - data link connection identifier

# Reference Documentation
- https://content.u-blox.com/sites/default/files/products/documents/MuxImplementation_ApplicationNote_%28UBX-13001887%29.pdf
- https://www.mikrocontroller.net/attachment/193104/MuxImplementation_ApplicationNote__WLS-CS-11002_.pdf
- https://github.com/ya-jeks/gsmmux/blob/master/gsm0710-driver.pdf
- https://www.3gpp.org/ftp/tsg_t/tsg_t/tsgt_04/docs/pdfs/TP-99119.pdf best doc
- https://www.etsi.org/deliver/etsi_ts/127000_127099/127010/17.00.00_60/ts_127010v170000p.pdf most updated!

## Additional reading material
- http://www.iot.com.tr/uploads/pdf/Telit_CMUX_Implementation_User_Guide_r6.pdf
- https://www.paoli.cz/out/media/HUAWEI%20Module%20CMUX%20Application%20Guide-V100R001_04(1).pdf
- https://www.dcs.ed.ac.uk/home/slipb/Bluetooth%20VCC%20Models/Documents/bluetooth_f1.pdf
- https://ptgmedia.pearsoncmg.com/imprint_downloads/informit/bookreg/9780130661067/cr_ch10.pdf


# Inspiration
- https://github.com/torvalds/linux/blob/master/drivers/tty/n_gsm.c
- https://github.com/ya-jeks/gsmmux/blob/master/gsm0710.c
- https://codesearch.isocpp.org/actcd19/main/g/gsm0710muxd/gsm0710muxd_1.13-3/src/gsm0710muxd.c
- https://github.com/ya-jeks/gsmmux/ - probably best


# LICENSE
This project is copyrighted and dual licensed as AGPL3 and Commercial, for commercial licenses please contact the author [Dror Gluska](https://www.linkedin.com/in/drorgl/).

```
CMUX/TS 0710 Client/Server
Copyright (C) 2022 Dror Gluska

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```