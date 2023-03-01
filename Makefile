all: _common _api _server _client _nc-vlc-client _radio_support

_common:
	make -C common 

_api:
	make -C api

_server:
	make -C server

_client:
	make -C client

_nc-vlc-client:
	make -C nc-vlc-client

_radio_support:
	make -C radio_support

clean:
	make -C common clean
	make -C server clean
	make -C api clean
	make -C client clean
	make -C nc-vlc-client clean
	make -C radio_support clean

install:
	make -C server install
	make -C client install
	make -C nc-vlc-client install
	make -C radio_support install


