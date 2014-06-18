#mime.py

import email
from email import utils
import re
import os

class Mime:

    def __init__(self, mail_addr, msg_uidl):
        self.file_name = './schoolmail/mail/' + mail_addr + '/' + msg_uidl + '.eml'
        self.save_file_path = './schoolmail/tmp/' + msg_uidl + '/'

    def open_msg(self, file_name):
        fp = open(file_name, "r")
        self.msg = email.message_from_file(fp)
		
    def get_date(self):
        if not "date" in msg:
            return None
        date_str = self.msg["date"].strip()
        try:
            return utils.mktime_tz(utils.parsedate_tz(date_str))
        except:
            return None
	
    def get_sender(self):
        self.sender = email.Header.decode_header(self.msg["from"])[0][0]
        #print(self.sender)
        
        return self.sender
		
    def get_to(self):
        return self.to
        #print(self.to)
	
        return self.to
		
    def get_cc(self):
        return self.cc
        #print(self.cc)
	
        return self.cc

    def get_header(self):
        to = self.msg["To"]
        cc = self.msg["Cc"]

        self.to = []
        self.cc = []
        
        if to != None:
            m = re.findall('([\w.-]+)@([\w.-]+)', to)
            for i in range(len(m)):
                self.to.append(m[i][0] + '@' + m[i][1])
            if len(self.to) == 1:
                self.to = self.to[0]
            else:
                self.to = ";".join(self.to)
            fp = open(self.save_file_path + "to" + ".txt", "wb+")
            fp.write(self.to)
            fp.close()

        if cc != None:
            m = re.findall('([\w.-]+)@([\w.-]+)', cc)

            for i in range(len(m)):
                self.cc.append(m[i][0] + '@' + m[i][1])
            if len(self.cc) == 1:
                self.cc = self.cc[0]
            else:
                self.cc = ";".join(self.cc)
            fp = open(self.save_file_path + "cc" + ".txt", "wb+")
            fp.write(self.cc)
            fp.close()
	
    def get_subject(self):
        self.subject = email.Header.decode_header(self.msg["subject"])[0][0].decode("gb2312")
        #print(self.subject)
        
        return self.subject
		
    def decode_body(self, mail):
        if mail.is_multipart():
            for part in mail.get_payload():
                self.decode_body(part)
        else:
            content_type = mail.get_content_type()
            main_type, sub_type = content_type.split("/", 1) 

            if main_type == "text":
                fp = open(self.save_file_path + sub_type + ".txt", "wb+")
                fp.write(mail.get_payload(decode=True))
                fp.close()

            elif main_type == "image":
                file_name = email.Header.decode_header(mail.get_filename())[0][0].decode("gb2312")
                content_disposition = None
                content_id = None
                if mail["content-disposition"] != None:
                    content_disposition = (mail["content-disposition"].split(";"))[0]
                if mail["content-id"] != None:
                    content_id = mail["content-id"][1: len(mail["content-id"]) - 1]

                if content_disposition == "inline" or mail["content-id"] != None:
                    fp = open(self.save_file_path + content_id, "wb+")
                    fp.write(mail.get_payload(decode=True))
                    fp.close()
                    self.inline_list.append(content_id)
                else:
                    fp = open(self.save_file_path + file_name, "wb+")
                    fp.write(mail.get_payload(decode=True))
                    fp.close()
                    self.attach_list.append(file_name)
            elif main_type == "application":
                #file_name = email.Header.decode_header(mail.get_filename())[0][0]
                file_name = email.Header.decode_header(mail.get_param("name"))[0][0]
                content_disposition = (mail["content-disposition"].split(";"))[0]

                if content_disposition == "attachment":
                    fp = open(self.save_file_path + file_name, "wb+")
                    fp.write(mail.get_payload(decode=True))
                    fp.close()
                    self.attach_list.append(file_name)
                elif content_disposition == "inline":
                    content_id = mail["content-id"][1: len(mail["content-id"]) - 1]
                    fp = open(self.save_file_path + content_id, "wb+")
                    fp.write(mail.get_payload(decode=True))
                    fp.close()
                    self.inline_list.append(content_id)
                    
            else:
                pass

    def save_attach_name(self):
        if len(self.attach_list) == 0:
            return;
        fp = open(self.save_file_path + "attach.txt", "wb+")
        fp.write(";".join(self.attach_list))
        fp.close()

    def link_inline_file(self):
        if len(self.inline_list) == 0:
            return;
        if not os.path.isfile(self.save_file_path + "html.txt"):
            return;
        
        fp = open(self.save_file_path + "html.txt", "rb")
        content = fp.read()
        fp.close()

        for i in range(0, len(self.inline_list)):
            content = content.replace("cid:" + self.inline_list[i], self.save_file_path + self.inline_list[i])

        fp = open(self.save_file_path + "html.txt", "wb+")
        fp.write(content)
        fp.close()
	
    def get_body(self):
        self.attach_list = []
        self.inline_list = []
        
        self.open_msg(self.file_name)
        self.get_header()
        self.decode_body(self.msg)

        self.save_attach_name()
        self.link_inline_file()
	
    def get_attachment(self):
        return self.attch_list

