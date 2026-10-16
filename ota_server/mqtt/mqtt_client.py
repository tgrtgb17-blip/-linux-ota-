import paho.mqtt.client as mqtt
import threading
import time

broker = "192.168.6.212"
port = 1883

class MQTTClient:
	def __init__(self):
		self.client = mqtt.Client()
		self.client.on_connect = self.on_connect
		self.client.on_message = self.on_message
		self.message = {}
		self.connected = False
		self.lock = threading.Lock()
	def on_connect(self,client,userdata,flag,rc):
		if rc == 0:
			print("mqtt连接成功");
			self.connected = True
			
			client.subscribe("ota/+/status",qos = 1)
			client.subscribe("ota/+/result",qos = 1)
		
			print("mqtt订阅成功")
		else:
			print("mqtt连接失败")
	def on_message(self,client,userdata,msg):
		print("mqtt收到了从",msg.topic,"发布的",msg.payload)
		with self.lock:
			if msg.topic not in self.message:
				self.message[msg.topic] = []
			self.message[msg.topic].append(msg.payload)
	def connect(self):
		print("正在连接mqtt")
		
		self.client.connect(broker,port,60)
		self.client.loop_start()
		while  not self.connected:
			time.sleep(0.1)
	def publish(self,topic,payload,qos = 1,retain = False):
		if  not self.connected:
			print("mqtt未连接")
			raise Exception("mqtt未连接");
		if isinstance(payload,str):
			payload = payload.encode()
		result =self.client.publish(topic,payload,qos,retain)
		time.sleep(0.1)
		if result.rc != mqtt. MQTT_ERR_SUCCESS: 
			raise Exception("mqtt发送失败")
	def wait_message(self,topic,timeout):
		start = time.time()
		while time.time() - start < timeout:
			with self.lock:
				if topic in self.message:
					if len(self.message[topic])> 0 :
						return self.message[topic].pop(0)
			time.sleep(0.1)
		return None
		
	def close(self):
		self.client.loop_stop()
		self.client.disconnect()



		
