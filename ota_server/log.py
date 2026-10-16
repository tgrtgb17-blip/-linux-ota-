import logging
import os

if not os.path.exists("log"):
	os.makedir("log")

logging.basicConfig(
	filename = "log/ota.log",
	level = logging.INFO,
	format = "%(asctime)s-%(levelname)s-%(message)s"
)
def log_info(msg):
	logging.info(msg)
def log_error(msg):
	logging.error(msg)
