import json

task_file = "task/task.json"

def task_load():
	with open(task_file,"r") as f:
		return json.load(f)

def get_task_device():
	task = task_load()
	return task["devices"]
def task_firmware():
	task = task_load()
	return (task["firmware"],
			task["version"])
