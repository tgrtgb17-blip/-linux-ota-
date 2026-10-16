import json

FILE = "devices/devices.json"

def device_load():
	with open( FILE,"r") as f:
	 	return json.load(f)
def device_save(devices):
	with open(FILE,"w") as f:
		json.dump(devices,f,indent = 4)
def device_update_version(device_id,version):
	device = device_load()
	for dev in device:
		if dev["id"] == device_id:
			dev["version"] = version
	device_save(device)	
		
