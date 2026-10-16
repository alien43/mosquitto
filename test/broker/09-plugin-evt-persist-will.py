#!/usr/bin/env python3

from mosq_test_helper import *

from broker_config import BrokerConfig, ListenerConfig, PluginConfig
from mosquitto_broker import MosquittoBroker

def do_test():
    connect_packet = mqtt_packets.gen_connect(
        "plugin-evt-persist-will",
        proto_ver=4,
        clean_session=True,
        will_topic="will/topic",
        will_payload=b"will-message",
    )
    connack_packet = mqtt_packets.gen_connack(rc=0, proto_ver=4)
    disconnect_packet = mqtt_packets.gen_disconnect(proto_ver=4)

    publish_will_add_packet = mqtt_packets.gen_publish("evt/persist/will/add", qos=0, payload="plugin-evt-persist-will")
    publish_will_delete_packet = mqtt_packets.gen_publish("evt/persist/will/delete", qos=0, payload="plugin-evt-persist-will")

    port = mosq_test.get_port()
    broker_config = BrokerConfig(
        listeners = [ ListenerConfig(port=port) ],
        plugins = [ PluginConfig(path=mosq_paths.test_plugin('plugin_evt_persist_will')) ],
        allow_anonymous=True,
    )
    broker = MosquittoBroker(config=broker_config)
    with broker:
        sub_sock = mosq_test.sub_helper(port, 'evt/persist/will/#')

        sock = mosq_test.do_client_connect(connect_packet, connack_packet, port=port)
        mosq_test.expect_packet(sub_sock, "publish will add", publish_will_add_packet)

        sock.send(disconnect_packet)
        sock.close()

        mosq_test.expect_packet(sub_sock, "publish will delete", publish_will_delete_packet)
        sub_sock.close()


do_test()
