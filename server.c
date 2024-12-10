#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>

#define PORT 8888

struct Product {
    char name[100];
    int quantity;
    double price;
};

struct Product products[100];
int product_count = 0;

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "GET") == 0 && strcmp(url, "/api/products") == 0) {
        char response[1024] = "[";
        for (int i = 0; i < product_count; i++) {
            char product[256];
            snprintf(product, sizeof(product), "{\"name\":\"%s\",\"quantity\":%d,\"price\":%.2f}", 
                     products[i].name, products[i].quantity, products[i].price);
            strcat(response, product);
            if (i < product_count - 1) {
                strcat(response, ",");
            }
        }
        strcat(response, "]");
        return MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_MUST_COPY);
    } else if (strcmp(method, "POST") == 0 && strcmp(url, "/api/products") == 0) {
        // Handle adding a new product
        char *json_data = NULL;
        struct MHD_PostProcessor *pp = MHD_create_post_processor(connection, 1024, NULL, NULL);
        MHD_post_process(pp, upload_data, *upload_data_size);
        MHD_destroy_post_processor(pp);
        
        // Here you would parse json_data to extract product details
        // For simplicity, we will just add a dummy product
        if (product_count < 100) {
            strcpy(products[product_count].name, "Sample Product");
            products[product_count].quantity = 10;
            products[product_count].price = 5.99;
            product_count++;
        }
        return MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    }
    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                               &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;

    printf("Server running on port %d\n", PORT);
    getchar(); // Wait for user input to stop the server

    MHD_stop_daemon(daemon);
    return 0;
}
