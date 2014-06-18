# pop3

import poplib
import db
import email
from email import utils
import os
import re
 
class POP3Server:
    def __init__(self, mail_addr = "", hostname = "", user="", passwd = "", is_ssl = 0):
        self.mail_addr = mail_addr
        self.hostname = hostname
        self.user = user
        self.passwd = passwd
        self.is_ssl = is_ssl

    def set_property(self, hostname = "", user = "", passwd = "", is_ssl = False):
        self.hostname = hostname
        self.user = user
        self.passwd = passwd
        self.is_ssl = is_ssl

    def login(self):
        try:
            if self.is_ssl != 0:
                self.pop_server = poplib.POP3_SSL(self.hostname)
            else:
                self.pop_server = poplib.POP3(self.hostname)

            self.pop_server.user(self.user)
            self.pop_server.pass_(self.passwd)
        except:
            return 0
        finally:
            return 1

    def del_msg(self, msg_id): # exception
        self.pop_server.dele(msg_id)

    def mail_reset(self):
        self.pop_server.rset()

    def pop_quit(self):
        self.pop_server.quit()

    def get_mail_count(self):
        (mail_count, size) = pop_server.stat()

        return mail_count

    def get_uidl(self, msg_id):
        msg_uidl = self.pop_server.uidl(msg_id)
        msg_uidl = msg_uidl.decode()
        response, msg_id, self.msg_uidl = msg_uidl.split(" ")

        return self.msg_uidl

    def get_stat(self):
        self.mail_count, self.mail_size = self.pop_server.stat()

    def get_msg(self, msg_id):
        charset = 'utf-8'
        response, lines, octets = self.pop_server.retr(msg_id)

        self.msg = email.message_from_string("\n".join(lines))
        self.msg_subject = ""
        length_subject = 0;
        try:
            length_subject = len(email.Header.decode_header(self.msg["Subject"]))
        except:
            pass
        finally:
            if length_subject > 0 and charset == email.Header.decode_header(self.msg["Subject"])[length_subject - 1][1]:
                self.msg_subject = email.Header.decode_header(self.msg["Subject"])[0][0]
            elif length_subject > 0:
                self.msg_subject = email.Header.decode_header(self.msg["Subject"])[0][0].decode("gbk")

            self.msg_date = self.msg["Date"]
            
            msg_sender_tuple = email.Header.decode_header(self.msg["From"])
            length = len(msg_sender_tuple)
            m = re.search('([\w.-]+)@([\w.-]+)', msg_sender_tuple[length - 1][0])
            if m != None:
                self.msg_sender = m.group()
            else:
                self.msg_sender = msg_sender_tuple[length - 1][0]

##            print('------------------------')
##            print(self.msg_subject)
##            print('************************')
##            if length_subject > 0:
##                print(email.Header.decode_header(self.msg["Subject"]))
##            print(length_subject)
##            print(charset)
##            if length_subject > 0:
##                print(email.Header.decode_header(self.msg["Subject"])[length_subject - 1][1]) 
##            #print(type(email.Header.decode_header(self.msg["Subject"])[0][0]))
##            print('************************')
##    ##        print(self.msg_sender)
##    ##        print('************************')
##    ##        print(self.msg_date)
##            print('------------------------')
        

    def get_subject(self):
        return self.msg_subject

    def get_sender(self):
        return self.msg_sender

    def get_date(self):
        return self.msg_date
        
    def get_msg_info(self, msg_id): #获取：uidl，主题，日期，发件人
        self.get_uidl(msg_id)

    def db_save(self):
        if self.msg_sender == None:
            self.msg_sender = ""
        if self.msg_date == None:
            self.msg_sender = ""
            
        db_client = db.dbase("./mail_db.db")
        db_client.db_open()
        db_client.db_insert(self.mail_addr, self.msg_subject, self.msg_sender, self.msg_date, 0, 0, self.msg_uidl)
        db_client.db_close()
        
    def save_msg(self, msg_id):
        try:
            self.get_msg_info(msg_id)
            self.get_msg(msg_id)

            #保存一封邮件的基本信息（主题，发件人，日期，uidl)到db
            self.db_save()
            #保存一封邮件到文件
            #print("./schoolmail/mail/" + self.mail_addr + "/" + self.msg_uidl)
            if not os.path.isfile("./schoolmail/mail/" + self.mail_addr + "/" + self.msg_uidl + ".eml"):
                fp = open("./schoolmail/mail/" + self.mail_addr + "/" + self.msg_uidl + ".eml", "w")
                fp.write(self.msg.as_string(unixfrom = 1))
                fp.close()
            else:
                self.flag = 1
        except:
            pass

    def download_msg(self):
        self.flag = 0
        try:
            self.get_stat()
            print('mail_count = ', self.mail_count)
            
            msg_id = self.mail_count;
            while msg_id > 0:
                if self.flag == 1:
                    break;
                self.save_msg(msg_id)
                msg_id -= 1;
        except:
            pass
        finally:
            self.pop_quit()


        
        
