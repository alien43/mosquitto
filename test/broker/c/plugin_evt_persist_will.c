#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <mosquitto/broker.h>
#include <mosquitto/broker_plugin.h>

MOSQUITTO_PLUGIN_DECLARE_VERSION(5);

static mosquitto_plugin_id_t *plg_id;


int callback_persist_will_add(int event, void *event_data, void *user_data)
{
	struct mosquitto_evt_persist_will_msg *ed = event_data;
	const char *clientid;

	(void)user_data;

	if(event != MOSQ_EVT_PERSIST_WILL_ADD){
		abort();
	}
	clientid = ed->data.clientid;
	mosquitto_broker_publish_copy(NULL, "evt/persist/will/add", (int)strlen(clientid), clientid, 0, false, NULL);

	return MOSQ_ERR_SUCCESS;
}


int callback_persist_will_delete(int event, void *event_data, void *user_data)
{
	struct mosquitto_evt_persist_will_msg *ed = event_data;
	const char *clientid;

	(void)user_data;

	if(event != MOSQ_EVT_PERSIST_WILL_DELETE){
		abort();
	}
	clientid = ed->data.clientid;
	mosquitto_broker_publish_copy(NULL, "evt/persist/will/delete", (int)strlen(clientid), clientid, 0, false, NULL);

	return MOSQ_ERR_SUCCESS;
}


int mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **user_data, struct mosquitto_opt *opts, int opt_count)
{
	(void)user_data;
	(void)opts;
	(void)opt_count;

	plg_id = identifier;

	mosquitto_callback_register(plg_id, MOSQ_EVT_PERSIST_WILL_ADD, callback_persist_will_add, NULL, NULL);
	mosquitto_callback_register(plg_id, MOSQ_EVT_PERSIST_WILL_DELETE, callback_persist_will_delete, NULL, NULL);

	return MOSQ_ERR_SUCCESS;
}


int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count)
{
	(void)user_data;
	(void)opts;
	(void)opt_count;

	mosquitto_callback_unregister(plg_id, MOSQ_EVT_PERSIST_WILL_ADD, callback_persist_will_add, NULL);
	mosquitto_callback_unregister(plg_id, MOSQ_EVT_PERSIST_WILL_DELETE, callback_persist_will_delete, NULL);

	return MOSQ_ERR_SUCCESS;
}
