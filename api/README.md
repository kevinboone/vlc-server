# libvlc-server-client

This is a C library for controlling the `vlc-server` audio player.
Applications that use this will also need to use `libvlc-server-common`.

The Makefile builds a static library. Application headers are in the
`include` directory.

Here is a simple example of the use of the library.

    #include <vlc-server/api-client.h>

    VSApiError = 0;
    const char *path = "/path/to/media/file";
    LibVlcServerClient *lvsc = libvlc_server_client_new (host, port);
    libvlc_server_client_play (client, &err_code, NULL, path);
    if (err_code)
      fprintf (stderr, "%s: can't play", path);
    libvlc_server_client_destroy (client);

