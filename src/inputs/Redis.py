import redis

from util.Keychain import Keychain

ID = "redis"

KEYS = Keychain(
    "Redis_input_keys",
    HOST="host",
    PORT="port",
    TOPIC="topic",
)


def create(config):
    host = config[KEYS.HOST]
    port = int(config[KEYS.PORT])
    topic = config[KEYS.TOPIC]

    return RedisInput(host, port, topic)


class RedisInput:
    def __init__(self, host, port, topic):
        self.host = host
        self.port = port
        self.topic = topic

    def begin(self, observer):
        client = redis.StrictRedis(host=self.host, port=self.port)
        sub = client.pubsub()
        sub.subscribe(self.topic)

        for message in sub.listen():
            if message["type"] != "message":
                continue
            observer(message["data"].decode("utf-8"))