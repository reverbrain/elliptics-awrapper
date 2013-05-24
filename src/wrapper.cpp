#include "wrapper.h"

#include <elliptics/session.hpp>

struct elliptics_data
{
	ioremap::elliptics::node node;
	ioremap::elliptics::session session;
};

static void elliptics_add_remote(struct elliptics_storage *storage, const char *address, int port)
{
	elliptics_data *data = reinterpret_cast<elliptics_data*>(storage->priv);

	data->node.add_remote(address, port);
}

static void elliptics_set_groups(struct elliptics_storage *storage, const int *groups, int groups_count)
{
	elliptics_data *data = reinterpret_cast<elliptics_data*>(storage->priv);

	dnet_session_set_groups(data->session.get_native(), groups, groups_count);
}

static int elliptics_copy_data(char * &destination, const char *source, size_t size)
{
	destination = reinterpret_cast<char *>(malloc(size));
	if (!destination) {
		return -ENOMEM;
	}

	memcpy(destination, source, size);
	return 0;
}

static int elliptics_get(struct elliptics_storage *storage,
	const char *id, uint32_t id_size, char **error,
	char **data, uint64_t *result_size,
	uint64_t offset, uint64_t size)
{
	elliptics_data *el_data = reinterpret_cast<elliptics_data*>(storage->priv);

	*error = NULL;
	*data = NULL;
	*result_size = 0;

	std::string key(id, id_size);

        ioremap::elliptics::session session = el_data->session.clone();
        ioremap::elliptics::async_read_result result = session.read_data(key, offset, size);
        result.wait();

        if (result.error()) {
		int err = elliptics_copy_data(*error,
			result.error().message().c_str(),
			result.error().message().size() + 1);

		if (err) {
			*error = NULL;
		}

		return result.error().code();
        }

        ioremap::elliptics::data_pointer file = result.get_one().file();

	int err = elliptics_copy_data(*data, file.data<char>(), file.size());
	if (err) {
		*error = NULL;
		return err;
	}

	*result_size = file.size();
	return 0;
}

static int elliptics_put(struct elliptics_storage *storage,
	const char *id, uint32_t id_size, char **error,
	const char *data, uint64_t size, uint64_t offset)
{
	elliptics_data *el_data = reinterpret_cast<elliptics_data*>(storage->priv);

	*error = NULL;

	std::string key(id, id_size);

        // File is known not to be changed during write_data call
        ioremap::elliptics::data_pointer file
                = ioremap::elliptics::data_pointer::from_raw(const_cast<char *>(data), size);

        ioremap::elliptics::session session = el_data->session.clone();
        ioremap::elliptics::async_write_result result = session.write_data(key, file, offset);
        result.wait();

	if (result.error()) {
		int err = elliptics_copy_data(*error,
			result.error().message().c_str(),
			result.error().message().size() + 1);

		if (err) {
			*error = NULL;
		}

		return result.error().code();
        }

	return 0;
}

extern "C" {

elliptics_storage *elliptics_storage_create(const char *logger_path, int verbosity)
{
	ioremap::elliptics::file_logger logger(logger_path, verbosity);
	ioremap::elliptics::node node(logger);
	ioremap::elliptics::session session(node);
	session.set_exceptions_policy(ioremap::elliptics::session::no_exceptions);

	elliptics_data data = { node, session };

	elliptics_storage *storage = NULL;

	try {
		storage = new elliptics_storage;
	} catch (...) {
		return NULL;
	}

	try {
		storage->priv = new elliptics_data(data);
	} catch (...) {
		delete storage;
		return NULL;
	}

	storage->add_remote = &elliptics_add_remote;
	storage->set_groups = &elliptics_set_groups;
	storage->get = &elliptics_get;
	storage->put = &elliptics_put;

	return storage;
}

void elliptics_storage_destroy(struct elliptics_storage *storage)
{
	delete reinterpret_cast<elliptics_data *>(storage->priv);
	delete storage;
}

void elliptics_data_destroy(void *data)
{
	free(data);
}

}
