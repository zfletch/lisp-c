#define COUNT_OF(arr) (sizeof(arr) / sizeof(*arr))

void *smalloc(size_t size);
void *scalloc(size_t num, size_t size);
void *srealloc(void *ptr, size_t size);
void error(const char *message);
