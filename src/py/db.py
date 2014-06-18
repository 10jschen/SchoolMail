#db.py

import sqlite3

class dbase:

    def __init__(self, db_name):
        self.db_name = db_name

    def db_open(self):
        self.db_con = sqlite3.connect(self.db_name)
        self.db_con.text_factory = str
        self.db_cursor = self.db_con.cursor()

    def db_close(self):
        self.db_con.close()

    def db_commit(self):
        self.db_con.commit()

    def db_insert(self, mail_addr, subject, sender, date, is_read, msg_state, msg_uidl):
        cmd_value = (mail_addr, subject, sender, date, is_read, msg_state, msg_uidl)
        try:
            self.db_cursor.execute("insert into msg_info values(?, ?, ?, ?, ?, ?, ?)", cmd_value)
            self.db_commit()
        except:
            pass

    def db_select(self, msg_uidl):
        self.db_cursor.execute("select subject from msg_info where msg_uidl=%s" % msg_uidl)
        self.db_commit()
        res = self.db_cursor.fetchone()

