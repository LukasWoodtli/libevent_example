#include <stdio.h>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/* https://github.com/azaky/libevent-http-server/blob/master/server-evhttp.c */

static void send_doc_cb(struct evhttp_request *req, void *arg) {
    struct evbuffer *evb = NULL;
    const char *uri = evhttp_request_get_uri(req);
    char *whole_path = NULL;
    int fd = -1, fsize;

    printf("Got a GET request for <%s>\n",  uri);

    evb = evbuffer_new();

    char *text = "Hello";
    int ret = evbuffer_add(evb, text, strlen(text));

    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/plain");
    evhttp_send_reply(req, 200, "OK", evb);
    goto done;

done:
    if (whole_path) {
        free(whole_path);
    }
    if (evb) {
        evbuffer_free(evb);
    }
}

int
main(int argc, char **argv)
{
    struct event_base *base;
    struct evhttp_bound_socket *handle;
    struct evhttp *http;

    int port = 8080;

    if (argc > 1) {
        port = atoi(argv[1]);
    }
    if (port<=0 || port>65535) {
        puts("Invalid port");
        return 1;
    }

    base = event_base_new();
    if (!base) {
        fprintf(stderr, "Couldn't open event base!");
        return 1;
    }

    http = evhttp_new(base);
    if (!http) {
        fprintf(stderr, "Couldn't create evhttp!");
        return 1;
    }

    evhttp_set_gencb(http, send_doc_cb, NULL);

    handle = evhttp_bind_socket_with_handle(http, "127.0.0.1", port);
    if (!handle) {
        fprintf(stderr, "Couldn't bind to port %d", port);
        return 1;
    }

    event_base_dispatch(base);
    return 0;
}