import logging
import logging.config


_config = {
    'version': 1,
    'formatters': {
        'simple': {
            'format': '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        },
        # 其他的 formatter
    },
    'handlers': {
        'console': {
            'class': 'logging.StreamHandler',
            'level': 'DEBUG',
            'formatter': 'simple'
        },
        'file': {
            'class': 'logging.FileHandler',
            'filename': 'scripts.log',
            'level': 'DEBUG',
            'formatter': 'simple'
        },
        # 其他的 handler
    },
    'loggers':{
        'SampleLogger': {
            'handlers': ['file'],
            'level': 'DEBUG',
        },
        'FileLogger': {
            # 既有 console Handler，还有 file Handler
            'handlers': ['console', 'file'],
            'level': 'DEBUG',
        },
        # 其他的 Logger
    }
}

class Logger():

    def __init__(self, config:dict=None):
        if config:
            logging.config.dictConfig(config)
        else:
            logging.config.dictConfig(_config)
            
    def get_logger(self, name:str=None):
        if name:
            return logging.getLogger(name)
        else:
            return logging.getLogger("SampleLogger")


def test():
    logger = Logger().get_logger()
    logger.info("abcd")
    logger.warn("dddd")

if __name__ == "__main__":
    test()