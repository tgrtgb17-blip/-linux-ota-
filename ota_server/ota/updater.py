from mqtt.mqtt_client import MQTTClient
import serial
import time
import struct
import json
import struct
def CRC16(data):
	crc  = 0xFFFF
	for byte in data:
		crc ^= byte
		for i in range(8):	
			if crc&0x0001:
				crc = (crc>>1)^ 0xA001
			else:
				crc>>=1
	return crc
class OTAupdate:
	def __init__(self,mqtt_client,device):
		self.mqtt = mqtt_client
		self.id = device["id"]
		self.base_topic = ("ota/"+self.id)
		self.command_topic = (self.base_topic+"/command")
		self.status_topic = (self.base_topic+"/status")
		self.result_topic = (self.base_topic+"/result")
		self.progress_topic = (self.base_topic+"/progress")
	def publish_command(self,payload):
			self.mqtt.publish(self.command_topic,payload)
	def publish_progress(self,payload):
		self.mqtt.publish(self.progress_topic,payload)
	def wait_message(self,timeout = 50):
		print(self.base_topic+"正在等待消息")
		start = time.time()
		while time.time() - start < timeout:
			data = self.mqtt.wait_message(self.result_topic,1)
			if data is None:
				continue
			print(self.base_topic,"接受到",data)
			return data
		raise Exception("未收到消息")
	def update_start(self):
		self.publish_command("upgrade")
		time.sleep(15)
		self.publish_command("OTA")
		ack = self.wait_message()
		if ack == b"start":
			return '0'
		elif ack == b"force":
			return '1'
	def update_version_num(self,version,pan,timeout = 20):
		if pan == '0':
			self.publish_command(version)
			ack = self.wait_message()
			if ack == b"same":
				raise Exception("版本相同，不需要升级")
			print(self.id+"开始升级")
			return 0,0
		elif pan == '1':
			self.publish_command(version)
			data = self.wait_message()
			text = data.decode().strip()
			parts = text.split(";")
			ackn = int(parts[0][5:])
			acka = int(parts[1][5:])
			print("断点的包号是",ackn)
			print("断点的地址是",acka)
			return ackn,acka
	def update_file(self,path,pan,ackn,acka):
		with open(path,"rb") as f:
			data = f.read()
		packet_size = 254
		if pan == '0':
			print("固件的大小为：",len(data))
			self.publish_command(len(data).to_bytes(4,'little'))
			num = 1
			addr = 0
		elif pan == '1':
			self.publish_command(len(data).to_bytes(4,'little'))
		
			num = ackn
			num =num + 1
			ackb = acka- 0x08002800
			addr = ackb
			pack_len = ackb/ackn
			if pack_len != packet_size:
				print("设备不匹配，请重新更新")
				return 2
		while addr < len(data):
			pack = data[addr:addr+packet_size]
			length = len(pack) 
			crc = CRC16(pack)
			packet = bytes([num]) + bytes([length]) + pack + crc.to_bytes(2,'little')
			print("num=",num) 
			print("len=",length) 
			print("crc=",hex(crc)) 
			print("发送数据长度=",len(packet))
			time.sleep(1)
			self.publish_progress(packet)
			time.sleep(1)
			ack = self.wait_message()
			if ack == b"error":
				raise Exception("校验错误")
			elif ack == b'lose':
				raise Exception("丢包") 
			elif ack == b"success":
				print("写入成功")
			else:
				raise Exception("未知回应，升级失败")
			addr+=length
			num+=1
		self.publish_progress(bytes([0]))  
		ack = self.wait_message()
		if ack == b"OK":
			print(self.id,"ota升级成功，即将进入app")
			
	def update_ota(self,version,path):
		try:
			pan = self.update_start()
			ackn,acka = self.update_version_num(version,pan)
			self.update_file(path,pan,ackn,acka)
		except Exception as e:
			print(self.id,"升级失败",e)
