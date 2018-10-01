class Keychain:
    def __init__(self, name, **kwargs):
        self.name = name
        self.__dict__.update(kwargs)

    def __hash__(self):
        return self.name.__hash__()

    def __eq__(self, other):
        return self.name == other