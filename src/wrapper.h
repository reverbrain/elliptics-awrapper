#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

struct elliptics_storage
{
	void (*set_groups)(struct elliptics_storage *storage, const int *groups, int groups_count);
	void (*add_remote)(struct elliptics_storage *storage, const char *address, int port);

	int (*get)(struct elliptics_storage *storage, const char *id, uint32_t id_size, char **error, char **data, uint64_t *result_size, uint64_t offset, uint64_t size);
	int (*put)(struct elliptics_storage *storage, const char *id, uint32_t id_size, char **error, const char *data, uint64_t size, uint64_t offset);

	void *priv;
};

struct elliptics_storage *elliptics_storage_create(const char *logger_path, int verbosity);
void elliptics_storage_destroy(struct elliptics_storage *storage);
void elliptics_data_destroy(void *data);


#ifdef __cplusplus
}
#endif
