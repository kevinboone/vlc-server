all: _common _api _server _client

_common:
	make -C common 

_api:
	make -C api

_server:
	make -C server

_client:
	make -C client

clean:
	make -C common clean
	make -C server clean
	make -C api clean
	make -C client clean

install:
	make -C server install
	make -C client install


