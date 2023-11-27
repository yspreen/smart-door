import json
import os
def file_or_dir_exists(filename):
    try:
        os.stat(filename)
        return True
    except OSError:
        return False

class ConfigManager:
    def __init__(self, filename='config.json'):
        self.filename = filename
        self.configs = self.load_configs()

    def load_configs(self):
        """Load the configuration from the file, if it exists."""
        if file_or_dir_exists(self.filename):
            with open(self.filename, 'r') as file:
                return json.load(file)
        else:
            return {}

    def get(self, name, default=""):
        """Get a config value by name."""
        return self.configs.get(name, default)

    def set(self, name, value):
        """Set a config value by name and persist it to disk."""
        self.configs[name] = value
        with open(self.filename, 'w') as file:
            json.dump(self.configs, file)


config_manager = ConfigManager()
