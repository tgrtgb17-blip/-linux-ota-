from ota.updater import OTAupdate
import json
import threading
from log import log_info,log_error
from devices.devices_manager import device_update_version,device_load
from task.task import *
from mqtt.mqtt_client import MQTTClient

def load_device():
    with open("devices/devices.json", "r") as f:
        return json.load(f)
def update_device(info,firmware,version):
	dev_id = info["id"]
	try:
		print(dev_id,"开始升级")
		log_info(dev_id+"开始升级")
		first = MQTTClient()
		first.connect()
		updater = OTAupdate(first,info)
		updater.update_ota(info["version"],firmware)
		device_update_version(dev_id,version)
	except Exception as e:
		print(info["id"],"升级失败",e)
		log_error(dev_id+"升级失败"+str(e))
def main():
	devices = get_task_device()
	firmware,version = task_firmware()
	device = load_device()
	thread = []
	print("任务设备",devices)
	print("设备列表",device)
	print("固件",firmware)
	print("目标版本",version)	
	dev = None
	for info in device:
		if info["id"] == devices["id"]:
			dev = info
			break
	if dev is not None:	 	
		print("开始升级" + dev["id"])
		t = threading.Thread(target=update_device,args=(dev,firmware,version))
		thread.append(t)
		t.start()
		for t in thread:
			t.join()
			print("所有设备升级完成")
if __name__ == "__main__":
    main()
